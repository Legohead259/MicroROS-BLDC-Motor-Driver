#ifndef micro_ros_bldc_h
#define micro_ros_bldc_h

#include <Arduino.h>
#include "foc_bldc.h"

// The micro_ros_platformio library provides the functions to communicate with ROS2
#include <micro_ros_platformio.h>

// These are core ROS2 libraries for creating nodes, publishers, and executors
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

// Get the custom actions form the `motor_interfaces` pacakge
#include "motor_interfaces/action/home.h"

// Get the custom messages from the `motor_interfaces` package
#include "motor_interfaces/msg/angular_velocity.h"
#include "motor_interfaces/msg/encoder_angle.h"
#include "motor_interfaces/msg/encoder_position.h"
#include "motor_interfaces/msg/motor_direction.h"

// Get the custom services from the `motor_interfaces` package
#include "motor_interfaces/srv/set_motor_direction.h"
#include "motor_interfaces/srv/set_target_velocity.h"
// #include "tutorial_interfaces/srv/add_three_ints.h"


// Define macros for checking function success
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){ while(1); }} // Blocking
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){ }} // Non-blocking

// Interface type format {package}__{interface [action/srv/msg]}__{InterfaceName}_{Additional [Response/Request]}

// Redeclare types for readability
using Home_Goal = motor_interfaces__action__Home_Goal;
using Home_Feedback = motor_interfaces__action__Home_Feedback;
using Home_Result = motor_interfaces__action__Home_Result;

using AngularVelocityMsg = motor_interfaces__msg__AngularVelocity;
using EncoderAngleMsg = motor_interfaces__msg__EncoderAngle;
using EncoderPositionMsg = motor_interfaces__msg__EncoderPosition;
using MotorDirectionMsg = motor_interfaces__msg__MotorDirection;

using SetMotorDirection_Request = motor_interfaces__srv__SetMotorDirection_Request;
using SetMotorDirection_Response = motor_interfaces__srv__SetMotorDirection_Response;
using SetTargetVelocity_Request = motor_interfaces__srv__SetTargetVelocity_Request;
using SetTargetVelocity_Response = motor_interfaces__srv__SetTargetVelocity_Response;

// Instantiate message objects
Home_Goal homeGoal;
Home_Feedback homefeedback;
Home_Result homeResult;

rcl_publisher_t angularVelocityPublisher;
rcl_publisher_t angularPositionPublisher;
rcl_publisher_t motorDirectionPublisher;
rcl_timer_t angularVelocityTimer;
rcl_timer_t angularPositionTimer;
rcl_timer_t motorDirectionTimer;
AngularVelocityMsg angularVelocityMsg;
EncoderAngleMsg angularPositionMsg;
MotorDirectionMsg motorDirectionMsg;

rcl_service_t setMotorDirectionService;
rcl_service_t setTargetVelocityService;
SetMotorDirection_Request setMotorDirectionRequest;
SetMotorDirection_Response setMotorDirectionResponse;
SetTargetVelocity_Request setTargetVelocityRequest;
SetTargetVelocity_Response setTargetVelocityResponse;

// Instantiate ROS2 base objects
rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;
rclc_executor_t executor;
rcl_timer_t motorControlTimer;


// ========================
// === ACTION CALLBACKS ===
// ========================


// ===========================
// === PUBLISHER CALLBACKS ===
// ===========================


// ============================
// === SUBSCRIBER CALLBACKS ===
// ============================


// =========================
// === SERVICE CALLBACKS ===
// =========================


void setMotorDirectionCallback(const void* req, void* res){
    SetMotorDirection_Request* req_in = (SetMotorDirection_Request*) req;
    SetMotorDirection_Response* res_in = (SetMotorDirection_Response*) res;

    // Set target velocity based on request direction
    // FALSE - counter-clockwise; TRUE - clockwise
    direction = req_in->direction;
    targetVelocity = !direction ? -targetVelocity : targetVelocity;

    // Send response back to client
    res_in->success = true;
}

void setTargetVelocityCallback(const void* req, void* res) {
    SetTargetVelocity_Request* req_in = (SetTargetVelocity_Request*) req;
    SetTargetVelocity_Response* res_in = (SetTargetVelocity_Response*) res;

    // Set target velocity base don request value
    targetVelocity = !direction ? -req_in->target : req_in->target;

    // Send response back to client
    res_in->success = true;
}


// =======================
// === TIMER CALLBACKS ===
// =======================


void motorControlCallback(rcl_timer_t* timer, int64_t last_call_time) {
    RCLC_UNUSED(last_call_time);
    if (timer != NULL) {
        motor.move(targetVelocity);
    }
}

void angularPositionCallback(rcl_timer_t * timer, int64_t last_call_time) {
    RCLC_UNUSED(last_call_time);
    if (timer != NULL) {
        RCSOFTCHECK(rcl_publish(&angularPositionPublisher, &angularPositionMsg, NULL));
        angularPositionMsg.timestamp = millis();
        angularPositionMsg.angle = sensor.getAngleEn() != 0 ? sensor.getAngleResult() : -999;
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
        ROSIDL_GET_MSG_TYPE_SUPPORT(motor_interfaces, msg, EncoderAngle),
        "/angular_position"));

    // Initialize servers
    RCCHECK(rclc_service_init_default(
        &setMotorDirectionService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(motor_interfaces, srv, SetMotorDirection), 
        "/set_motor_direction"));

    RCCHECK(rclc_service_init_default(
        &setTargetVelocityService, 
        &node, 
        ROSIDL_GET_SRV_TYPE_SUPPORT(motor_interfaces, srv, SetTargetVelocity), 
        "/set_target_velocity"));

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