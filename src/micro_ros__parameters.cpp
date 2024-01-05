#include "micro_ros__parameters.h"

// =============================
// === PARAMETER DEFINITIONS ===
// =============================


#ifdef ARDUINO_ARCH_ESP32
Preferences parameterSettings;
#endif // ARDUINO_ARCH_ESP32

rclc_parameter_server_t parameterService;
parameter_t controllerMode = {.key=PARAM_NAME__CONTROLLER_MODE, .type=RCLC_PARAMETER_INT};
parameter_t polePairs{.key=PARAM_NAME__POLE_PAIRS, .type=RCLC_PARAMETER_INT};
parameter_t phaseResistance{.key=PARAM_NAME__PHASE_RESISTANCE, .type=RCLC_PARAMETER_DOUBLE};
parameter_t kvRating{.key=PARAM_NAME__KV_RATING, .type=RCLC_PARAMETER_INT};
parameter_t phaseInductance{.key=PARAM_NAME__PHASE_INDUCTANCE, .type=RCLC_PARAMETER_DOUBLE};
parameter_t velocityP{.key=PARAM_NAME__VELOCITY_P, .type=RCLC_PARAMETER_DOUBLE};
parameter_t velocityI{.key=PARAM_NAME__VELOCITY_I, .type=RCLC_PARAMETER_DOUBLE};
parameter_t velocityD{.key=PARAM_NAME__VELOCITY_D, .type=RCLC_PARAMETER_DOUBLE};
parameter_t velocityOutputRamp{.key=PARAM_NAME__VELOCITY_OUTPUT_RAMP, .type=RCLC_PARAMETER_DOUBLE};
parameter_t velocityLPFTf{.key=PARAM_NAME__VELOCITY_LPF_TF, .type=RCLC_PARAMETER_DOUBLE};
parameter_t angleP{.key=PARAM_NAME__ANGLE_P, .type=RCLC_PARAMETER_DOUBLE};
parameter_t angleI{.key=PARAM_NAME__ANGLE_I, .type=RCLC_PARAMETER_DOUBLE};
parameter_t angleD{.key=PARAM_NAME__ANGLE_D, .type=RCLC_PARAMETER_DOUBLE};
parameter_t angleOutputRamp{.key=PARAM_NAME__ANGLE_OUTPUT_RAMP, .type=RCLC_PARAMETER_DOUBLE};
parameter_t angleLPFTf{.key=PARAM_NAME__ANGLE_LPF_TF, .type=RCLC_PARAMETER_DOUBLE};
parameter_t driverVoltageLimit{.key=PARAM_NAME__DRIVER_VOLTAGE_LIMIT, .type=RCLC_PARAMETER_DOUBLE};
parameter_t driverCurrentLimit{.key=PARAM_NAME__DRIVER_CURRENT_LIMIT, .type=RCLC_PARAMETER_DOUBLE};
parameter_t motorVoltageLimit{.key=PARAM_NAME__MOTOR_VOLTAGE_LIMIT, .type=RCLC_PARAMETER_DOUBLE};
parameter_t motorCurrentLimit{.key=PARAM_NAME__MOTOR_CURRENT_LIMIT, .type=RCLC_PARAMETER_DOUBLE};
parameter_t motorVelocityLimit{.key=PARAM_NAME__MOTOR_VELOCITY_LIMIT, .type=RCLC_PARAMETER_DOUBLE};

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
    &driverCurrentLimit,
    &motorVoltageLimit,
    &motorCurrentLimit,
    &motorVelocityLimit
};

void initializeParameterService() {
    #ifdef ARDUINO_ARCH_ESP32
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
        parameterSettings.putDouble(PARAM_NAME__DRIVER_CURRENT_LIMIT, 0.2);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_VOLTAGE_LIMIT, 7);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_CURRENT_LIMIT, 0.2);
        parameterSettings.putDouble(PARAM_NAME__MOTOR_VELOCITY_LIMIT, 10);
    }
    Serial1.println("Found parameter file"); // Debug
    Serial1.println("Loading parameters into memory..."); // Debug
    // Iterate through the parameter array in memory and put loaded values there
    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) {
        Serial1.printf("Loading key: %s...\t", params[i]->key); // Debug

        if (params[i]->type == RCLC_PARAMETER_BOOL) {
            params[i]->bool_value = parameterSettings.getBool(params[i]->key);
            Serial1.printf("Got boolean parameter!\tValue: %s\r\n", params[i]->bool_value ? "TRUE" : "FALSE"); // Debug
        }
        else if (params[i]->type == RCLC_PARAMETER_INT) {
            params[i]->integer_value = parameterSettings.getInt(params[i]->key);
            Serial1.printf("Got integer parameter!\tValue: %d\r\n", params[i]->integer_value); // Debug
        } 
        else if (params[i]->type == RCLC_PARAMETER_DOUBLE) {
            params[i]->double_value = parameterSettings.getDouble(params[i]->key);
            Serial1.printf("Got double parameter!\tValue: %0.3f\r\n", params[i]->double_value); // Debug
        }
        else {
            params[i]->type = RCLC_PARAMETER_NOT_SET;
        }
    }
    #endif // ARDUINO_ARCH_ESP32

    // TODO: Support out NVS systems for other MCUs

    // Add parameters to the server
    Serial1.println("Adding parameters to server...");
    for (uint8_t i = 0; i < NUM_PARAMETERS; i++) { // Iterate through array of parameter structures
        if (params[i]->type == RCLC_PARAMETER_NOT_SET) { continue; } // Check for bad parameters

        RCSOFTCHECK(rclc_add_parameter(&parameterService, params[i]->key, params[i]->type));
        Serial1.printf("Added %s of type: %d\r\n", params[i]->key, params[i]->type); // Debug

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

        Serial1.printf("Set value of %s\r\n", params[i]->key); // Debug
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
    else {
    //     Serial1.printf("Parameter %s modified.", oldParam->name.data); // Debug
    //     switch (oldParam->value.type) {
    //     case RCLC_PARAMETER_BOOL:
    //         Serial1.printf(
    //         " Old value: %d, New value: %d (bool)", oldParam->value.bool_value,
    //         newParam->value.bool_value); // Debug
    //         break;
    //     case RCLC_PARAMETER_INT:
    //         Serial1.printf(
    //         " Old value: %ld, New value: %ld (int)", oldParam->value.integer_value,
    //         newParam->value.integer_value); // Debug
    //         break;
    //     case RCLC_PARAMETER_DOUBLE:
    //         Serial1.printf(
    //         " Old value: %f, New value: %f (double)", oldParam->value.double_value,
    //         newParam->value.double_value); // Debug
    //         break;
    //     default:
    //         break;
    //     }
    //     printf("\n");

        for (uint8_t i=0; i<NUM_PARAMETERS; i++) { // Iterate through the parameter array
            if (strcmp(newParam->name.data, PARAM_NAME__CONTROLLER_MODE) == 0) {
                Serial1.println("Editing controller mode"); // DEBUG
            }
        }
    }


    return true;
}
