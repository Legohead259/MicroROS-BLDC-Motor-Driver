#ifndef MICRO_ROS__PARAMETERS_H
#define MICRO_ROS__PARAMETERS_H

#include "micro_ros__definitions.h"

// ============================
// === PARAMETER DEFINTIONS ===
// ============================


#ifdef ARDUINO_ARCH_ESP32
extern Preferences parameterSettings;
#endif // ARDUINO_ARCH_ESP32

// NOTE: Due to restrictions within the Preferences library, parameter names are limited to 16 characters!

#define PARAM_NAME__CONTROLLER_MODE         "controller_mode"
#define PARAM_NAME__POLE_PAIRS              "pole_pairs"
#define PARAM_NAME__PHASE_RESISTANCE        "phase_r"
#define PARAM_NAME__KV_RATING               "kv_rating"
#define PARAM_NAME__PHASE_INDUCTANCE        "phase_l"
#define PARAM_NAME__VELOCITY_P              "velocity_p"
#define PARAM_NAME__VELOCITY_I              "velocity_i"
#define PARAM_NAME__VELOCITY_D              "velocity_d"
#define PARAM_NAME__VELOCITY_OUTPUT_RAMP    "velocity_ramp"
#define PARAM_NAME__VELOCITY_LPF_TF         "velocity_lpf_tf"
#define PARAM_NAME__ANGLE_P                 "angle_p"
#define PARAM_NAME__ANGLE_I                 "angle_i"
#define PARAM_NAME__ANGLE_D                 "angle_d"
#define PARAM_NAME__ANGLE_OUTPUT_RAMP       "angle_ramp"
#define PARAM_NAME__ANGLE_LPF_TF            "angle_lpf_tf"
#define PARAM_NAME__DRIVER_VOLTAGE_LIMIT    "driver_v_limit"
#define PARAM_NAME__DRIVER_CURRENT_LIMIT    "driver_i_limit"
#define PARAM_NAME__MOTOR_VOLTAGE_LIMIT     "motor_v_limit"
#define PARAM_NAME__MOTOR_CURRENT_LIMIT     "motor_i_limit"
#define PARAM_NAME__MOTOR_VELOCITY_LIMIT    "motor_vel_limit"

#define NUM_PARAMETERS 20

struct parameter_t; // Forward declare the struct

typedef std::function<void(parameter_t*)> ParameterChangeHandler;

typedef struct parameter_t {
    const char* key;
    rclc_parameter_type_t type;
    bool bool_value;
    int64_t integer_value;
    double double_value;
    ParameterChangeHandler onChangePtr;

    void onChange() { if (onChangePtr != nullptr) onChangePtr(this); }
} parameter_t;

extern parameter_t controllerMode;
extern parameter_t polePairs;
extern parameter_t phaseResistance;
extern parameter_t kvRating;
extern parameter_t phaseInductance;
extern parameter_t velocityP;
extern parameter_t velocityI;
extern parameter_t velocityD;
extern parameter_t velocityOutputRamp;
extern parameter_t velocityLPFTf;
extern parameter_t angleP;
extern parameter_t angleI;
extern parameter_t angleD;
extern parameter_t angleOutputRamp;
extern parameter_t angleLPFTf;
extern parameter_t driverVoltageLimit;
extern parameter_t driverCurrentLimit;
extern parameter_t motorVoltageLimit;
extern parameter_t motorCurrentLimit;
extern parameter_t motorVelocityLimit;
extern parameter_t* params[NUM_PARAMETERS];


// Initialize parameter server options
const rclc_parameter_options_t parameterServiceOpts = {
    .notify_changed_over_dds = true,        // Publish parameter events to other ROS 2 nodes as well.
    .max_params = NUM_PARAMETERS,           // Maximum number of parameters allowed on the rclc_parameter_server_t object.
    .allow_undeclared_parameters = false,   // Allows creation of parameters from external parameter clients.
    .low_mem_mode = false                   // Reduces the memory used by the parameter server, applies constraints
};

void initializeParameterService();


// ==================================
// === PARAMETER CHANGE CALLBACKS ===
// ==================================


void controllerModeChangeCallback(parameter_t* param);
void polePairsChangeCallback(parameter_t* param);
void phaseResistanceChangeCallback(parameter_t* param);
void kvRatingChangeCallback(parameter_t* param);
void phaseInductanceChangeCallback(parameter_t* param);
void velocityPChangeCallback(parameter_t* param);
void velocityIChangeCallback(parameter_t* param);
void velocityDChangeCallback(parameter_t* param);
void velocityRampChangeCallback(parameter_t* param);
void velocityLPFChangeCallback(parameter_t* param);
void anglePChangeCallback(parameter_t* param);
void angleIChangeCallback(parameter_t* param);
void angleDChangeCallback(parameter_t* param);
void angleRampChangeCallback(parameter_t* param);
void angleLFPChangeCallback(parameter_t* param);
void driverVoltageLimitChangeCallback(parameter_t* param);
void driverCurrentLimitChangeCallback(parameter_t* param);
void motorVoltageLimitChangeCallback(parameter_t* param);
void motorCurrentLimitChangeCallback(parameter_t* param);
void motorVelocityLimitChangeCallback(parameter_t* param);

#endif // MICRO_ROS__PARAMETERS_H