#include "micro_ros_bldc.h"

rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;
rcl_timer_t neopixelTimer;
TaskHandle_t MicroROSTask;
AgentState agentState = WAITING_AGENT;

bool createPublishers() {
    RCCHECK(rclc_publisher_init_default(
        &angularPositionPublisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(motor_interfaces, msg, AngularMeasurement),
        "/angular_position"));

    return true;
}

bool createSubscribers() {
    RCCHECK(rclc_subscription_init_best_effort(
        &joySubscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Joy),
        "joy"));

    Serial1.print("Setting subscriber memory..."); // Debug
    // Initialize subscriber message memory
    micro_ros_utilities_memory_conf_t conf = {
        .max_ros2_type_sequence_capacity = 20,
        .max_basic_type_sequence_capacity = 20,
    };
    Serial1.print(micro_ros_utilities_create_message_memory(
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Joy),
        &joystickMsg,
        conf));
    Serial1.println("done!"); // Debug

    return true;
}

bool createServices() {
    RCCHECK(rclc_service_init_default(
        &deviceIdentifyService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, Trigger), 
        "/device_identify"));

    RCCHECK(rclc_service_init_default(
        &setTargetService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(motor_interfaces, srv, SetTarget), 
        "/set_motor_target"));

    RCCHECK(rclc_service_init_default(
        &enableMotorService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, Trigger), 
        "/enable_motor"));

    RCCHECK(rclc_service_init_default(
        &disableMotorService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, Trigger), 
        "/disable_motor"));

    RCCHECK(rclc_parameter_server_init_with_option(
        &parameterService, 
        &node,
        &parameterServiceOpts));

    return true;
}

bool addServices() {
    RCCHECK(rclc_executor_add_service(
        &executor, 
        &deviceIdentifyService, 
        &deviceIdentifyRequest, 
        &deviceIdentifyResponse, 
        deviceIdentifyCallback));
    
    RCCHECK(rclc_executor_add_service(
        &executor, 
        &setTargetService, 
        &setTargetRequest, 
        &setTargetResponse, 
        setTargetCallback));

    RCCHECK(rclc_executor_add_service(
        &executor, 
        &enableMotorService, 
        &enableMotorRequest, 
        &enableMotorResponse, 
        enableMotorCallback));

    RCCHECK(rclc_executor_add_service(
        &executor, 
        &disableMotorService, 
        &disableMotorRequest, 
        &disableMotorResponse, 
        disableMotorCallback));

    RCCHECK(rclc_executor_add_parameter_server(
        &executor, 
        &parameterService, 
        onParameterChangedCallback));

    return true; 
}

bool createTimers() {
    RCCHECK(rclc_timer_init_default(
        &angularMeasurementTimer,
        &support,
        RCL_MS_TO_NS(10),
        angularMeasurementCallback));

    return true;
}

bool addTimers() {
    RCCHECK(rclc_executor_add_timer(
        &executor, 
        &angularMeasurementTimer));

    return true;
}

bool addSubscribers() {
    RCCHECK(rclc_executor_add_subscription(
        &executor, 
        &joySubscriber, 
        &joystickMsg, 
        &joystickCallback, 
        ON_NEW_DATA));

    return true;
}

bool createEntities() {
    // Get the default memory allocator provided by rcl
    allocator = rcl_get_default_allocator();

    // Initialize rclc_support with default allocator
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

    // Initialize a ROS node with the name "bldc_node"
    RCCHECK(rclc_node_init_default(&node, "bldc_node", "", &support));

    // Create application components
    createPublishers();
    createSubscribers();
    createServices();
    createTimers();

    // Create executor
    RCCHECK(rclc_executor_init(&executor, &support.context, 10+RCLC_EXECUTOR_PARAMETER_SERVER_HANDLES, &allocator));
    addServices();
    addTimers();
    addSubscribers();

    initializeParameterService();

    return true;
}

void destroyEntities() {
    rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
    (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

    RCSOFTCHECK(rcl_publisher_fini(&angularPositionPublisher, &node));
    RCSOFTCHECK(rcl_timer_fini(&angularMeasurementTimer));
    RCSOFTCHECK(rcl_timer_fini(&neopixelTimer));
    RCSOFTCHECK(rcl_service_fini(&setControllerModeService, &node));
    RCSOFTCHECK(rcl_service_fini(&deviceIdentifyService, &node));
    RCSOFTCHECK(rcl_service_fini(&setTargetService, &node));
    RCSOFTCHECK(rcl_service_fini(&enableMotorService, &node));
    RCSOFTCHECK(rcl_service_fini(&disableMotorService, &node));
    RCSOFTCHECK(rclc_parameter_server_fini(&parameterService, &node));
    RCSOFTCHECK(rclc_executor_fini(&executor));
    RCSOFTCHECK(rcl_node_fini(&node));
    RCSOFTCHECK(rclc_support_fini(&support));
}

void microROSTaskCallback(void* parameters) {
    Serial1.printf("MicroROS running on Core %d\r\n", xPortGetCoreID()); // Debug
    for(;;) {
        
        // Handle system LED
        ledStateMachine.executeState();

        // Handle Micro-ROS tasking
        switch (agentState) {
            case WAITING_AGENT:
                EXECUTE_EVERY_N_MS(500, agentState = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_AVAILABLE : WAITING_AGENT;);
                break;

            case AGENT_AVAILABLE:
                agentState = createEntities() ? AGENT_CONNECTED : WAITING_AGENT; // Check if entities are properly created
                if (agentState == AGENT_CONNECTED) { // Update system state
                    changeSystemState(IDLE_WITH_CONNECTION);
                    motor.enable();
                }
                if (agentState == WAITING_AGENT) { // If entities are not properly created, destroy them
                    destroyEntities();
                    motor.disable();
                };
                break;
            
            case AGENT_CONNECTED:
                EXECUTE_EVERY_N_MS(200, agentState = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_CONNECTED : AGENT_DISCONNECTED;);
                if (agentState == AGENT_CONNECTED) {
                    // Execute pending tasks in the executor. This will handle all ROS communications.
                    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
                }
                break;

            case AGENT_DISCONNECTED:
                destroyEntities();
                agentState = WAITING_AGENT;
                changeSystemState(IDLE_NO_CONNECTION);
                motor.disable();
                break;
                
            default:
                break;
        }
    }
}

void microROSNodeSetup() {
    xTaskCreatePinnedToCore(
        microROSTaskCallback, /* Function to implement the task */
        "microROS", /* Name of the task */
        25000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &MicroROSTask,  /* Task handle. */
        0); /* Core where the task should run */

    set_microros_serial_transports(Serial); // Configure Micro-ROS library to use Arduino serial
}