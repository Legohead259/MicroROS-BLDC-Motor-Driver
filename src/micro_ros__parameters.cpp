#include "micro_ros__parameters.h"

// =============================
// === PARAMETER DEFINITIONS ===
// =============================


#ifdef ARDUINO_ARCH_ESP32
Preferences parameterSettings;
#endif // ARDUINO_ARCH_ESP32

rclc_parameter_server_t parameterService;
parameter_t controllerMode = {
    .key=PARAM_NAME__CONTROLLER_MODE, 
    .type=RCLC_PARAMETER_INT,
    .onChangePtr=controllerModeChangeCallback
};
parameter_t polePairs{
    .key=PARAM_NAME__POLE_PAIRS, 
    .type=RCLC_PARAMETER_INT,
    .onChangePtr=polePairsChangeCallback
};
parameter_t phaseResistance{
    .key=PARAM_NAME__PHASE_RESISTANCE, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=phaseResistanceChangeCallback
};
parameter_t kvRating{
    .key=PARAM_NAME__KV_RATING, 
    .type=RCLC_PARAMETER_INT,
    .onChangePtr=kvRatingChangeCallback
};
parameter_t phaseInductance{
    .key=PARAM_NAME__PHASE_INDUCTANCE, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=phaseInductanceChangeCallback
};
parameter_t velocityP{
    .key=PARAM_NAME__VELOCITY_P, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=velocityPChangeCallback
};
parameter_t velocityI{
    .key=PARAM_NAME__VELOCITY_I,
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=velocityIChangeCallback
};
parameter_t velocityD{
    .key=PARAM_NAME__VELOCITY_D, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=velocityDChangeCallback
};
parameter_t velocityOutputRamp{
    .key=PARAM_NAME__VELOCITY_OUTPUT_RAMP, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=velocityRampChangeCallback
};
parameter_t velocityLPFTf{
    .key=PARAM_NAME__VELOCITY_LPF_TF, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=velocityLPFChangeCallback
};
parameter_t angleP{
    .key=PARAM_NAME__ANGLE_P, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=anglePChangeCallback
};
parameter_t angleI{
    .key=PARAM_NAME__ANGLE_I, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=angleIChangeCallback
};
parameter_t angleD{
    .key=PARAM_NAME__ANGLE_D, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=angleDChangeCallback
};
parameter_t angleOutputRamp{
    .key=PARAM_NAME__ANGLE_OUTPUT_RAMP, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=angleRampChangeCallback
};
parameter_t angleLPFTf{
    .key=PARAM_NAME__ANGLE_LPF_TF, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=angleLFPChangeCallback
};
parameter_t driverVoltageLimit{
    .key=PARAM_NAME__DRIVER_VOLTAGE_LIMIT, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=driverVoltageLimitChangeCallback
};
parameter_t motorVoltageLimit{
    .key=PARAM_NAME__MOTOR_VOLTAGE_LIMIT, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=motorVoltageLimitChangeCallback
};
parameter_t motorCurrentLimit{
    .key=PARAM_NAME__MOTOR_CURRENT_LIMIT, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=motorCurrentLimitChangeCallback
};
parameter_t motorVelocityLimit{
    .key=PARAM_NAME__MOTOR_VELOCITY_LIMIT, 
    .type=RCLC_PARAMETER_DOUBLE,
    .onChangePtr=motorVelocityLimitChangeCallback
};

parameter_t* params[NUM_PARAMETERS] = {
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
    &motorVoltageLimit,
    &motorCurrentLimit,
    &motorVelocityLimit
};

#ifdef ARDUINO_ARCH_ESP32
void loadPreferences() {
    parameterSettings.begin("uros_params", false);
    
    // parameterSettings.clear(); // DEBUG

    if (!parameterSettings.isKey(PARAM_NAME__CONTROLLER_MODE)) { // Check for first run and initialize with defaults
        Serial1.println("Did not find parameter file. Initializing defaults...");

        parameterSettings.putInt(PARAM_NAME__CONTROLLER_MODE, VELOCITY_OPEN_LOOP);
        parameterSettings.putInt(PARAM_NAME__POLE_PAIRS, 7);
        parameterSettings.putDouble(PARAM_NAME__PHASE_RESISTANCE, 3.0);
        parameterSettings.putInt(PARAM_NAME__KV_RATING, 600);
        parameterSettings.putDouble(PARAM_NAME__PHASE_INDUCTANCE, 3.0);
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
        parameterSettings.putDouble(PARAM_NAME__MOTOR_VOLTAGE_LIMIT, 7);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_CURRENT_LIMIT, 0.2);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_VELOCITY_LIMIT, 10);
    }
    // Serial1.println("Found parameter file"); // Debug
    // Serial1.println("Loading parameters into memory..."); // Debug
    // Iterate through the parameter array in memory and put loaded values there
    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) {
        // Serial1.printf("Loading key: %s...\t", params[i]->key); // Debug

        if (params[i]->type == RCLC_PARAMETER_BOOL) {
            params[i]->bool_value = parameterSettings.getBool(params[i]->key);
            // Serial1.printf("Got boolean parameter!\tValue: %s\r\n", params[i]->bool_value ? "TRUE" : "FALSE"); // Debug
        }
        else if (params[i]->type == RCLC_PARAMETER_INT) {
            params[i]->integer_value = parameterSettings.getInt(params[i]->key);
            // Serial1.printf("Got integer parameter!\tValue: %d\r\n", params[i]->integer_value); // Debug
        } 
        else if (params[i]->type == RCLC_PARAMETER_DOUBLE) {
            params[i]->double_value = parameterSettings.getDouble(params[i]->key);
            // Serial1.printf("Got double parameter!\tValue: %0.3f\r\n", params[i]->double_value); // Debug
        }
        else {
            params[i]->type = RCLC_PARAMETER_NOT_SET;
        }
    }
}
#endif // ARDUINO_ARCH_ESP32

void initializeParameterService() {
    #ifdef ARDUINO_ARCH_ESP32
    loadPreferences();
    #endif // ARDUINO_ARCH_ESP32

    // TODO: Support out NVS systems for other MCUs

    // Add parameters to the server
    Serial1.println("Adding parameters to server...");
    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) { // Iterate through array of parameter structures
        if (params[i]->type == RCLC_PARAMETER_NOT_SET) { continue; } // Check for bad parameters

        RCSOFTCHECK(rclc_add_parameter(&parameterService, params[i]->key, params[i]->type));

        switch (params[i]->type) {
            case RCLC_PARAMETER_BOOL:
                RCSOFTCHECK(rclc_parameter_set_bool(
                    &parameterService, 
                    params[i]->key, 
                    params[i]->bool_value));
                break;
            case RCLC_PARAMETER_INT:
                RCSOFTCHECK(rclc_parameter_set_int(
                    &parameterService, 
                    params[i]->key, 
                    params[i]->integer_value));
                break;
            case RCLC_PARAMETER_DOUBLE:
                RCSOFTCHECK(rclc_parameter_set_double(
                    &parameterService, 
                    params[i]->key, 
                    params[i]->double_value));
                break;
        }
    }
}

bool onParameterChangedCallback(const Parameter* oldParam, const Parameter* newParam, void* context) {
    (void) context;

    if (oldParam == NULL && newParam == NULL) { // Check for callback error
        Serial1.printf("Callback error, both parameters are NULL\n"); // Debug
        return false;
    }

    if (oldParam == NULL) { // Check for new parameter
        Serial1.printf("Creating new parameter %s\n", newParam->name.data); // Debug
        return false; // Do not allow new parameters
    } 
    else if (newParam == NULL) { // Check for deleting parameter
        Serial1.printf("Deleting parameter %s\n", oldParam->name.data); // Debug
        return false; // Do not allow deleting parameters
    } 
    else { // Check for changing parameters
        // Serial1.printf("Checking for: %s\r\n", newParam->name.data); // Debug
        for (uint8_t i=0; i<NUM_PARAMETERS; i++) { // Iterate through the parameter array
            Serial1.printf("\t Comparing against: %s\r\n", params[i]->key);
            if (strcmp(newParam->name.data, params[i]->key) == 0) { // Check if parameter is in array
                // Serial1.printf("Editing %s\r\n", params[i]->key); // DEBUG
                switch (newParam->value.type) { // Check parameter type and set new value accordingly
                    case RCLC_PARAMETER_BOOL:
                        params[i]->bool_value = newParam->value.bool_value;
                        break;
                    case RCLC_PARAMETER_INT:
                        params[i]->integer_value = newParam->value.integer_value;
                        break;
                    case RCLC_PARAMETER_DOUBLE:
                        params[i]->double_value = newParam->value.double_value;
                        break;
                    default:
                        break;
                }
                params[i]->onChange(); // Execute the parameters change function
                saveParam(params[i]);
                return true;
            }
        }
        return false;
    }

    return true;
}

void saveParam(parameter_t* param) {
    // TODO: replace parameterSettings call with generic "save bool/int/double" callback
    #ifdef ARDUINO_ARCH_ESP32
    switch (param->type) {
        case RCLC_PARAMETER_BOOL:
            parameterSettings.putBool(param->key, param->bool_value);
            break;
        case RCLC_PARAMETER_INT:
            parameterSettings.putInt(param->key, param->integer_value);
            break;
        case RCLC_PARAMETER_DOUBLE:
            parameterSettings.putDouble(param->key, param->double_value);
            break;
        default:
            break;
    }
    #endif // ARDUINO_ARCH_ESP32
}


// ==================================
// === PARAMETER CHANGE CALLBACKS ===
// ==================================


void controllerModeChangeCallback(parameter_t* param) {
    uint8_t _mode = param->integer_value;
    bool _isClosedLoop = _mode < SetControlMode_ModeCodes::POSITION_OPEN_LOOP;
    bool _isTorqueControl = _mode >= SetControlMode_ModeCodes::TORQUE_VOLTAGE && _mode <= SetControlMode_ModeCodes::TORQUE_FOC_CURRENT;

    // Check for motor failure mode
    if (motor.motor_status == FOCMotorStatus::motor_error ||
        motor.motor_status == FOCMotorStatus::motor_calib_failed ||
        motor.motor_status == FOCMotorStatus::motor_init_failed) {
        
        // res_in->result = SetControlMode_ResultCodes::FAILURE_FOC_ERROR;
        return;
    }

    // Check if requested mode is closed-loop and feedback sensor initialized
    if ((_isClosedLoop && !angleSensorInitialized)) {
        // Account for torque control without angle sensor, but with current sensor
        if (!(_isTorqueControl && currentSensorInitialized)) {
            // res_in->result = SetControlMode_ResultCodes::FAILURE_FEEDBACK_ERROR;
            return;
        }
    }

    // Set controller mode
    switch (_mode) {
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
        // res_in->result = SetControlMode_ResultCodes::FAILURE_UNSPECIFIED;
        return;
    }

    // Return successful result
    // res_in->result = SetControlMode_ResultCodes::SUCCESS;
    Serial1.println("Set motor control mode Using callback!");
}

void polePairsChangeCallback(parameter_t* param) {
    motor.pole_pairs = param->integer_value;
    Serial1.println("Set pole pairs Using callback!");
}

void phaseResistanceChangeCallback(parameter_t* param) {
    motor.phase_resistance = param->double_value;
}

void kvRatingChangeCallback(parameter_t* param) {
    motor.KV_rating = param->integer_value;
}

void phaseInductanceChangeCallback(parameter_t* param) {
    motor.phase_inductance = param->double_value;
}

void velocityPChangeCallback(parameter_t* param) {
    motor.PID_velocity.P = param->double_value;
}

void velocityIChangeCallback(parameter_t* param) {
    motor.PID_velocity.I = param->double_value;
}

void velocityDChangeCallback(parameter_t* param) {
    motor.PID_velocity.D = param->double_value;
}

void velocityRampChangeCallback(parameter_t* param) {
    motor.PID_velocity.output_ramp = param->double_value;
}

void velocityLPFChangeCallback(parameter_t* param) {
    motor.LPF_velocity.Tf = param->double_value;
}

void anglePChangeCallback(parameter_t* param) {
    motor.P_angle.P = param->double_value;
}

void angleIChangeCallback(parameter_t* param) {
    motor.P_angle.I = param->double_value;
}

void angleDChangeCallback(parameter_t* param) {
    motor.P_angle.D = param->double_value;
}

void angleRampChangeCallback(parameter_t* param) {
    motor.P_angle.output_ramp = param->double_value;
}

void angleLFPChangeCallback(parameter_t* param) {
    motor.LPF_angle.Tf = param->double_value;
}

void driverVoltageLimitChangeCallback(parameter_t* param) {
    driver.voltage_limit = param->double_value;
}

void motorVoltageLimitChangeCallback(parameter_t* param) {
    motor.voltage_limit = param->double_value;
}

void motorCurrentLimitChangeCallback(parameter_t* param) {
    motor.current_limit = param->double_value;
}

void motorVelocityLimitChangeCallback(parameter_t* param) {
    motor.voltage_limit = param->double_value;
}