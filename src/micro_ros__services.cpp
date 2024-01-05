#include "micro_ros__services.h"


// =============================
// === PARAMETER DEFINITIONS ===
// =============================


#ifdef ARDUINO_ARCH_ESP32
Preferences parameterSettings;
#endif // ARDUINO_ARCH_ESP32

parameter_int_t controllerMode(PARAM_NAME__CONTROLLER_MODE);
parameter_int_t polePairs(PARAM_NAME__POLE_PAIRS);
parameter_double_t phaseResistance(PARAM_NAME__PHASE_RESISTANCE);
parameter_int_t kvRating(PARAM_NAME__KV_RATING);
parameter_double_t phaseInductance(PARAM_NAME__PHASE_INDUCTANCE);
parameter_double_t velocityP(PARAM_NAME__VELOCITY_P);
parameter_double_t velocityI(PARAM_NAME__VELOCITY_I);
parameter_double_t velocityD(PARAM_NAME__VELOCITY_D);
parameter_double_t velocityOutputRamp(PARAM_NAME__VELOCITY_OUTPUT_RAMP);
parameter_double_t velocityLPFTf(PARAM_NAME__VELOCITY_LPF_TF);
parameter_double_t angleP(PARAM_NAME__ANGLE_P);
parameter_double_t angleI(PARAM_NAME__ANGLE_I);
parameter_double_t angleD(PARAM_NAME__ANGLE_D);
parameter_double_t angleOutputRamp(PARAM_NAME__ANGLE_OUTPUT_RAMP);
parameter_double_t angleLPFTf(PARAM_NAME__ANGLE_LPF_TF);
parameter_double_t driverVoltageLimit(PARAM_NAME__DRIVER_VOLTAGE_LIMIT);
parameter_double_t driverCurrentLimit(PARAM_NAME__DRIVER_CURRENT_LIMIT);
parameter_double_t motorVoltageLimit(PARAM_NAME__MOTOR_VOLTAGE_LIMIT);
parameter_double_t motorCurrentLimit(PARAM_NAME__MOTOR_CURRENT_LIMIT);
parameter_double_t motorVelocityLimit(PARAM_NAME__MOTOR_VELOCITY_LIMIT);

parameter_base_t* params[NUM_PARAMETERS] = {
    &controllerMode,
    &polePairs,
    &phaseResistance,
    &kvRating,
    &phaseInductance,
    &velocityP,
    &velocityI,
    &velocityD,
    &velocityOutputRamp,
    &velocityLPFTf,
    &angleP,
    &angleI,
    &angleD,
    &angleOutputRamp,
    &angleLPFTf,
    &driverVoltageLimit,
    &driverCurrentLimit,
    &motorVoltageLimit,
    &motorCurrentLimit,
    &motorVelocityLimit
};


// =======================
// === SERVICE OBJECTS ===
// =======================


rcl_service_t setControllerModeService;
rcl_service_t deviceIdentifyService;
rcl_service_t setTargetService;
rcl_service_t enableMotorService;
rcl_service_t disableMotorService;
rclc_parameter_server_t parameterService;
SetControllerMode_Request setControllerModeRequest;
SetControllerMode_Response setControllerModeResponse;
Trigger_Request deviceIdentifyRequest;
Trigger_Response deviceIdentifyResponse;
SetTarget_Request setTargetRequest;
SetTarget_Response setTargetResponse;
Trigger_Request enableMotorRequest;
Trigger_Request enableMotorResponse;
Trigger_Request disableMotorRequest;
Trigger_Request disableMotorResponse;


// =========================
// === SERVICE FUNCTIONS ===
// =========================


void initializeParameterService() {
    #ifdef ARDUINO_ARCH_ESP32
    parameterSettings.begin("uros_params", false);
    
    if (!parameterSettings.isKey(PARAM_NAME__CONTROLLER_MODE)) { // Check for first run and initialize with defaults
        Serial1.println("Did not find parameter file. Initializing defaults...");

        parameterSettings.putInt(PARAM_NAME__CONTROLLER_MODE, VELOCITY_OPEN_LOOP);
        parameterSettings.putInt(PARAM_NAME__POLE_PAIRS, 7);
        parameterSettings.putDouble(PARAM_NAME__PHASE_RESISTANCE, 3);
        parameterSettings.putInt(PARAM_NAME__KV_RATING, 600);
        parameterSettings.putDouble(PARAM_NAME__PHASE_INDUCTANCE, 3);

        parameterSettings.putDouble(PARAM_NAME__VELOCITY_P, 0.5);
        parameterSettings.putDouble(PARAM_NAME__VELOCITY_I, 10.0);
        parameterSettings.putDouble(PARAM_NAME__VELOCITY_D, 0);
        parameterSettings.putDouble(PARAM_NAME__VELOCITY_OUTPUT_RAMP, 300);
        parameterSettings.putDouble(PARAM_NAME__VELOCITY_LPF_TF, 5);

        parameterSettings.putDouble(PARAM_NAME__ANGLE_P, 20);
        parameterSettings.putDouble(PARAM_NAME__ANGLE_I, 0);
        parameterSettings.putDouble(PARAM_NAME__ANGLE_D, 0);
        parameterSettings.putDouble(PARAM_NAME__ANGLE_OUTPUT_RAMP, 1e6);
        parameterSettings.putDouble(PARAM_NAME__ANGLE_LPF_TF, 0);

        parameterSettings.putDouble(PARAM_NAME__DRIVER_VOLTAGE_LIMIT, 7);
        parameterSettings.putDouble(PARAM_NAME__DRIVER_CURRENT_LIMIT, 0.2);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_VOLTAGE_LIMIT, 7);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_CURRENT_LIMIT, 0.2);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_VELOCITY_LIMIT, 10);
    }
    Serial1.println("Found parameter file"); // Debug
    Serial1.println("Loading parameters into memory..."); // Debug
    // Iterate through the parameter array in memory and put loaded values there
    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) {
        if (typeid(*params[i]) == typeid(parameter_bool_t)) {
            Serial1.println("Got boolean parameter!"); // Debug
            dynamic_cast<parameter_bool_t*>(params[i])->value = parameterSettings.getBool(params[i]->key);
            params[i]->type = RCLC_PARAMETER_BOOL;
        }
        else if (typeid(*params[i]) == typeid(parameter_int_t)) {
            Serial1.println("Got integer parameter!"); // Debug
            dynamic_cast<parameter_int_t*>(params[i])->value = parameterSettings.getInt(params[i]->key);
            params[i]->type = RCLC_PARAMETER_INT;
        } 
        else if (typeid(*params[i]) == typeid(parameter_double_t)) {
            Serial1.println("Got double parameter!"); // Debug
            dynamic_cast<parameter_double_t*>(params[i])->value = parameterSettings.getDouble(params[i]->key);
            params[i]->type = RCLC_PARAMETER_DOUBLE;
        }
        else {
            params[i]->type = RCLC_PARAMETER_NOT_SET;
        }
    }
    #endif // ARDUINO_ARCH_ESP32

    // Add parameters to the server
    Serial1.println("Adding parameters to server...");
    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) { // Iterate through array of parameter structures
        if (params[i]->type == RCLC_PARAMETER_NOT_SET) { continue; } // Check for bad parameters

        RCSOFTCHECK(rclc_add_parameter(&parameterService, params[i]->key, params[i]->type));
        Serial1.printf("Added %s of type: %d \r\n", params[i]->key, params[i]->type); // Debug

        switch (params[i]->type) {
            case RCLC_PARAMETER_BOOL:
                RCSOFTCHECK(rclc_parameter_set_bool(
                    &parameterService, 
                    params[i]->key, 
                    dynamic_cast<parameter_bool_t*>(params[i])->value));
                break;
            case RCLC_PARAMETER_INT:
                RCSOFTCHECK(rclc_parameter_set_int(
                    &parameterService, 
                    params[i]->key, 
                    dynamic_cast<parameter_int_t*>(params[i])->value));
                break;
            case RCLC_PARAMETER_DOUBLE:
                RCSOFTCHECK(rclc_parameter_set_double(
                    &parameterService, 
                    params[i]->key, 
                    dynamic_cast<parameter_double_t*>(params[i])->value));
                break;
        }

        Serial1.printf("Set value of %s\r\n", params[i]->key); // Debug
    }
}


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

void enableMotorCallback(const void* req, void* res) {
    Trigger_Request* req_in = (Trigger_Request*) req;
    Trigger_Response* res_in = (Trigger_Response*) res;

    motor.enable();
    res_in->success = motor.enabled;
}

void disableMotorCallback(const void* req, void* res) {
    Trigger_Request* req_in = (Trigger_Request*) req;
    Trigger_Response* res_in = (Trigger_Response*) res;

    motor.disable();
    res_in->success = !motor.enabled;
}

bool onParameterChangedCallback(const Parameter* oldParam, const Parameter* newParam, void* context) {
    (void) context;

    // if (oldParam == NULL && newParam == NULL) { // Check for callback error
    //     // printf("Callback error, both parameters are NULL\n");
    //     return false;
    // }

    // if (oldParam == NULL) {
    //     printf("Creating new parameter %s\n", newParam->name.data);
    // } else if (newParam == NULL) {
    //     printf("Deleting parameter %s\n", oldParam->name.data);
    // } else {
    //     printf("Parameter %s modified.", oldParam->name.data);
    //     switch (oldParam->value.type) {
    //     case RCLC_PARAMETER_BOOL:
    //         printf(
    //         " Old value: %d, New value: %d (bool)", oldParam->value.bool_value,
    //         newParam->value.bool_value);
    //         break;
    //     case RCLC_PARAMETER_INT:
    //         printf(
    //         " Old value: %ld, New value: %ld (int)", oldParam->value.integer_value,
    //         newParam->value.integer_value);
    //         break;
    //     case RCLC_PARAMETER_DOUBLE:
    //         printf(
    //         " Old value: %f, New value: %f (double)", oldParam->value.double_value,
    //         newParam->value.double_value);
    //         break;
    //     default:
    //         break;
    //     }
    //     printf("\n");
    // }

    return true;
}
