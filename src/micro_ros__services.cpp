#include "micro_ros__services.h"


// =======================
// === SERVICE OBJECTS ===
// =======================


rcl_service_t setControllerModeService;
rcl_service_t deviceIdentifyService;
rcl_service_t setTargetService;
SetControllerMode_Request setControllerModeRequest;
SetControllerMode_Response setControllerModeResponse;
Trigger_Request deviceIdentifyRequest;
Trigger_Response deviceIdentifyResponse;
SetTarget_Request setTargetRequest;
SetTarget_Response setTargetResponse;


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

void deviceIdentifyCallback(const void* req, void* res) {
    Trigger_Request* req_in = (Trigger_Request*) req;
    Trigger_Response* res_in = (Trigger_Response*) res;

    changeSystemState(DEVICE_IDENTIFY);

    // Send response back to client
    res_in->success = true;
}

void setTargetCallback(const void* req, void* res) {
    SetTarget_Request* req_in = (SetTarget_Request*) req;
    SetTarget_Response* res_in = (SetTarget_Response*) res;

    // Set target velocity based on request value
    target = req_in->target;

    // Update system state from target
    if (target != 0) {
        systemState = (target > 0) ? PARTIAL_FORWARD : PARTIAL_REVERSE;
    }
    // TODO: Set and check maxes (FULL_FORWARD, FULL_REVERSE)
    else {
        systemState = IDLE_WITH_CONNECTION;
    }

    // Send response back to client
    res_in->result = true;
}
