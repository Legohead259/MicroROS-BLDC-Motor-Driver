#ifndef MICRO_ROS__SERVICES_H
#define MICRO_ROS__SERVICES_H

#include "micro_ros__definitions.h"

// Instantiate service objects
rcl_service_t setControllerModeService;
rcl_service_t setMotorDirectionService;
rcl_service_t setTargetVelocityService;
SetControllerMode_Request setControllerModeRequest;
SetControllerMode_Response setControllerModeResponse;
SetMotorDirection_Request setMotorDirectionRequest;
SetMotorDirection_Response setMotorDirectionResponse;
SetTarget_Request setTargetVelocityRequest;
SetTarget_Response setTargetVelocityResponse;

// =========================
// === SERVICE CALLBACKS ===
// =========================


void setControllerModeCallback(const void* req, void* res) {
    SetControllerMode_Request* req_in = (SetControllerMode_Request*) req;
    SetControllerMode_Response* res_in = (SetControllerMode_Response*) res;

    uint8_t _mode = req_in->mode;
    bool _isClosedLoop = _mode < SetControlMode_ModeCodes::POSITION_OPEN_LOOP;
    bool _isTorqueControl = _mode >= SetControlMode_ModeCodes::TORQUE_VOLTAGE && _mode <= SetControlMode_ModeCodes::TORQUE_FOC_CURRENT;

    // Check for motor failure mode
    if (motor.motor_status == FOCMotorStatus::motor_error ||
        motor.motor_status == FOCMotorStatus::motor_calib_failed ||
        motor.motor_status == FOCMotorStatus::motor_init_failed) {
        
        res_in->result = SetControlMode_ResultCodes::FAILURE_FOC_ERROR;
        return;
    }

    // Check if requested mode is closed-loop and feedback sensor initialized
    if ((_isClosedLoop && !angleSensorInitialized)) {
        // Account for torque control without angle sensor, but with current sensor
        if (!(_isTorqueControl && currentSensorInitialized)) {
            res_in->result = SetControlMode_ResultCodes::FAILURE_FEEDBACK_ERROR;
            return;
        }
    }

    // Set controller mode
    switch (req_in->mode) {
    case SetControlMode_ModeCodes::POSITION_CLOSED_LOOP:
        motor.controller = MotionControlType::angle;
        break;
    case SetControlMode_ModeCodes::VELOCITY_CLOSED_LOOP:
        motor.controller = MotionControlType::velocity;
        break;
    case SetControlMode_ModeCodes::TORQUE_VOLTAGE:
    case SetControlMode_ModeCodes::TORQUE_CURRENT:
    case SetControlMode_ModeCodes::TORQUE_FOC_CURRENT:
        motor.controller = MotionControlType::torque;
        break;
    case SetControlMode_ModeCodes::POSITION_OPEN_LOOP:
        motor.controller = MotionControlType::angle_openloop;
        break;
    case SetControlMode_ModeCodes::VELOCITY_OPEN_LOOP:
        motor.controller = MotionControlType::velocity_openloop;
        break;
    default:
        res_in->result = SetControlMode_ResultCodes::FAILURE_UNSPECIFIED;
        return;
    }

    // Return successful result
    res_in->result = SetControlMode_ResultCodes::SUCCESS;
}

void setMotorDirectionCallback(const void* req, void* res) {
    SetMotorDirection_Request* req_in = (SetMotorDirection_Request*) req;
    SetMotorDirection_Response* res_in = (SetMotorDirection_Response*) res;

    // Set target velocity based on request direction
    // FALSE - counter-clockwise; TRUE - clockwise
    direction = req_in->direction;
    target = !direction ? -target : target;

    // Send response back to client
    res_in->result = true;
}

void setTargetCallback(const void* req, void* res) {
    SetTarget_Request* req_in = (SetTarget_Request*) req;
    SetTarget_Response* res_in = (SetTarget_Response*) res;

    // Set target velocity base don request value
    target = !direction ? -req_in->target : req_in->target;

    // Send response back to client
    res_in->result = true;
}

#endif // MICRO_ROS__SERVICES_H