#ifndef micro_ros_bldc_h
#define micro_ros_bldc_h

#include "micro_ros__actions.h"
#include "micro_ros__pubsub.h"
#include "micro_ros__services.h"


// =======================
// === TIMER CALLBACKS ===
// =======================


void motorControlCallback(rcl_timer_t* timer, int64_t last_call_time) {
    RCLC_UNUSED(last_call_time);
    if (timer != NULL) {
        motor.move(target);
    }
}


// ======================
// === NODE FUNCTIONS ===
// ======================


void microROSNodeSetup() {
    set_microros_serial_transports(Serial); // Configure Micro-ROS library to use Arduino serial
    delay(2000); // Allow some time for everything to start properly

    // Get the default memory allocator provided by rcl
    allocator = rcl_get_default_allocator();

    // Initialize rclc_support with default allocator
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

    // Initialize a ROS node with the name "bldc_node"
    RCCHECK(rclc_node_init_default(&node, "bldc_node", "", &support));

    // Initialize publishers
    RCCHECK(rclc_publisher_init_default(
        &angularPositionPublisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(motor_interfaces, msg, AngularPosition),
        "/angular_position"));

    // Initialize services
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

    // Initialize timers
    RCCHECK(rclc_timer_init_default(
        &angularPositionTimer,
        &support,
        RCL_MS_TO_NS(10),
        angularPositionCallback));

    RCCHECK(rclc_timer_init_default(
        &motorControlTimer,
        &support,
        RCL_MS_TO_NS(1),
        motorControlCallback));

    // Initialize an executor that will manage the execution of all the ROS entities (publishers, subscribers, services, timers)
    RCCHECK(rclc_executor_init(&executor, &support.context, 4, &allocator));

    // Add servers
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
        setTargetVelocityCallback));

    // Add timers
    RCCHECK(rclc_executor_add_timer(&executor, &angularPositionTimer));
    RCCHECK(rclc_executor_add_timer(&executor, &motorControlTimer));
}

#endif // micro_ros_bldc_h