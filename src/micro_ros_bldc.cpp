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

bool createServices() {
    RCCHECK(rclc_service_init_default(
        &setControllerModeService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(motor_interfaces, srv, SetControllerMode), 
        "/set_controller_mode"));

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

    return true;
}

bool addServices() {
    RCCHECK(rclc_executor_add_service(
        &executor, 
        &setControllerModeService, 
        &setControllerModeRequest, 
        &setControllerModeResponse, 
        setControllerModeCallback));

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
    RCCHECK(rclc_executor_add_timer(&executor, &angularMeasurementTimer));

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
    createServices();
    createTimers();

    // Create executor
    RCCHECK(rclc_executor_init(&executor, &support.context, 10, &allocator));
    addServices();
    addTimers();

    return true;
}

void destroyEntities() {
    rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
    (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

    rcl_publisher_fini(&angularPositionPublisher, &node);
    rcl_timer_fini(&angularMeasurementTimer);
    rcl_timer_fini(&neopixelTimer);
    rcl_service_fini(&setTargetService, &node);
    rclc_executor_fini(&executor);
    rcl_node_fini(&node);
    rclc_support_fini(&support);
}

void microROSTaskCallback(void* parameters) {
    Serial1.printf("MicroROS running on Core %d\r\n", xPortGetCoreID());
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
                }
                if (agentState == WAITING_AGENT) { // If entities are not properly created, destroy them
                    destroyEntities();
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