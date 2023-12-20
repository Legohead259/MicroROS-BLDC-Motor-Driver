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
        ROSIDL_GET_MSG_TYPE_SUPPORT(motor_interfaces, msg, AngularPosition),
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
        &setMotorDirectionService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(motor_interfaces, srv, SetMotorDirection), 
        "/set_motor_direction"));

    RCCHECK(rclc_service_init_default(
        &setTargetVelocityService, 
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
        &setMotorDirectionService, 
        &setMotorDirectionRequest, 
        &setMotorDirectionResponse, 
        setMotorDirectionCallback));
    
    RCCHECK(rclc_executor_add_service(
        &executor, 
        &setTargetVelocityService, 
        &setTargetVelocityRequest, 
        &setTargetVelocityResponse, 
        setTargetCallback));

    return true; 
}

bool createTimers() {
    RCCHECK(rclc_timer_init_default(
        &angularPositionTimer,
        &support,
        RCL_MS_TO_NS(10),
        angularPositionCallback));

    // RCCHECK(rclc_timer_init_default(
    //     &neopixelTimer,
    //     &support,
    //     RCL_MS_TO_NS(10),
    //     neopixelTimerCallback));

    return true;
}

bool addTimers() {
    RCCHECK(rclc_executor_add_timer(&executor, &angularPositionTimer));
    // RCCHECK(rclc_executor_add_timer(&executor, &neopixelTimer));

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
    // createServices();
    createTimers();

    // Create executor
    RCCHECK(rclc_executor_init(&executor, &support.context, 10, &allocator));
    // addServices();
    addTimers();

    return true;
}

void destroyEntities() {
    rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
    (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

    rcl_publisher_fini(&angularPositionPublisher, &node);
    rcl_timer_fini(&angularPositionTimer);
    rcl_timer_fini(&neopixelTimer);
    // TODO: destroy services
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
                agentState = (true == createEntities()) ? AGENT_CONNECTED : WAITING_AGENT;
                if (agentState == WAITING_AGENT) {
                    destroyEntities();
                };
                break;
            
            case AGENT_CONNECTED:
                EXECUTE_EVERY_N_MS(200, agentState = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_CONNECTED : AGENT_DISCONNECTED;);
                if (agentState == AGENT_CONNECTED) {
                    // Execute pending tasks in the executor. This will handle all ROS communications.
                    systemState = IDLE_WITH_CONNECTION;
                    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
                }
                break;

            case AGENT_DISCONNECTED:
                destroyEntities();
                agentState = WAITING_AGENT;
                systemState = IDLE_NO_CONNECTION;
                break;
                
            default:
                break;
        }

        if (agentState == AGENT_CONNECTED) {
            digitalWrite(LED_PIN, 1);
        } else {
            digitalWrite(LED_PIN, 0);
        }
    }
}

void neopixelTimerCallback(rcl_timer_t * timer, int64_t last_call_time) {
    ledStateMachine.executeState();
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