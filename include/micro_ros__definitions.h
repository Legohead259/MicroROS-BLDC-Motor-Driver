#ifndef MICRO_ROS__DEFINITIONS_H
#define MICRO_ROS__DEFINITIONS_H

#include <Arduino.h>
#include "foc_bldc.h"
#include "state_machine.h"

// The micro_ros_platformio library provides the functions to communicate with ROS2
#include <micro_ros_platformio.h>

// These are core ROS2 libraries for creating nodes, publishers, and executors
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

// Define macros for checking function success
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){ while(1); }} // Blocking
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){ }} // Non-blocking

// Define macros for executing a task every interval
#define EXECUTE_EVERY_N_MS(MS, X)  do { \
    static volatile int64_t init = -1; \
    if (init == -1) { init = uxr_millis();} \
    if (uxr_millis() - init > MS) { X; init = uxr_millis();} \
} while (0)\

// Get the custom actions form the `motor_interfaces` package
#include "motor_interfaces/action/home.h"

// Get the custom messages from the `motor_interfaces` package
#include "motor_interfaces/msg/angular_acceleration.h"
#include "motor_interfaces/msg/angular_velocity.h"
#include "motor_interfaces/msg/angular_position.h"
#include "motor_interfaces/msg/motor_status.h"

// Get the custom services from the `motor_interfaces` package
#include "motor_interfaces/srv/set_controller_mode.h"
#include "motor_interfaces/srv/set_motor_direction.h"
#include "motor_interfaces/srv/set_target.h"

// Interface type format {package}__{interface [action/srv/msg]}__{InterfaceName}_{Additional [Response/Request]}

// Redeclare types for readability
using Home_Goal = motor_interfaces__action__Home_Goal;
using Home_Feedback = motor_interfaces__action__Home_Feedback;
using Home_Result = motor_interfaces__action__Home_Result;

using AngularAccelerationMsg = motor_interfaces__msg__AngularAcceleration;
using AngularVelocityMsg = motor_interfaces__msg__AngularVelocity;
using AngularPositionMsg = motor_interfaces__msg__AngularPosition;
using MotorStatusMsg = motor_interfaces__msg__MotorStatus;

using SetControllerMode_Request = motor_interfaces__srv__SetControllerMode_Request;
using SetControllerMode_Response = motor_interfaces__srv__SetControllerMode_Response;
using SetMotorDirection_Request = motor_interfaces__srv__SetMotorDirection_Request;
using SetMotorDirection_Response = motor_interfaces__srv__SetMotorDirection_Response;
using SetTarget_Request = motor_interfaces__srv__SetTarget_Request;
using SetTarget_Response = motor_interfaces__srv__SetTarget_Response;

// Instantiate ROS2 base objects
extern rcl_allocator_t allocator;
extern rclc_support_t support;
extern rcl_node_t node;
extern rclc_executor_t executor;
extern rcl_timer_t neopixelTimer;

// Define various parameters

enum AgentState {
    WAITING_AGENT,
    AGENT_AVAILABLE,
    AGENT_CONNECTED,
    AGENT_DISCONNECTED
};

extern AgentState agentState;

enum SetControlMode_ModeCodes : uint8_t {
    POSITION_CLOSED_LOOP    = 1,    // Maintain a target position (in rad), using the position sensor feedback
    VELOCITY_CLOSED_LOOP    = 2,    // Maintain a target velocity (in rad/sec), using the position sensor feedback
    TORQUE_VOLTAGE          = 3,    // Use a target voltage to maintain an approximated torque
    TORQUE_CURRENT          = 4,    // Use a target current and estimated current to maintain an approximated torque
    TORQUE_FOC_CURRENT      = 5,    // Use a target current and current feedback sensors to maintain a known torque
    POSITION_OPEN_LOOP      = 6,    // Maintain a target position based on back EMF readings
    VELOCITY_OPEN_LOOP      = 7,    // Maintain a target velocity based on back EMF readings
};

enum SetControlMode_ResultCodes : uint8_t {
    SUCCESS                 = 0,    // Controller operating mode successfully changed
    FAILURE_UNSPECIFIED     = 1,    // Failed to change modes - unspecified error
    FAILURE_FOC_ERROR       = 2,    // Failed to change mode due to FOC system being in error state
    FAILURE_FEEDBACK_ERROR  = 3     // Failed to change mode since required feedback device is not initialized or in error state
};

#endif // MICRO_ROS__DEFINITIONS_H