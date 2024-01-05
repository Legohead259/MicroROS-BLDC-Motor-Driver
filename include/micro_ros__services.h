#ifndef MICRO_ROS__SERVICES_H
#define MICRO_ROS__SERVICES_H

#include "micro_ros__definitions.h"


// ============================
// === PARAMETER DEFINTIONS ===
// ============================


#ifdef ARDUINO_ARCH_ESP32
extern Preferences parameterSettings;
#endif // ARDUINO_ARCH_ESP32

#define PARAM_NAME__CONTROLLER_MODE     "controller_mode"
#define PARAM_NAME__POLE_PAIRS          "pole_pairs"
#define PARAM_NAME__PHASE_RESISTANCE    "phase_resistance"
#define PARAM_NAME__KV_RATING           "kv_rating"
#define PARAM_NAME__PHASE_INDUCTANCE    "phase_inductance"

#define PARAM_NAME__VELOCITY_P              "velocity_p"
#define PARAM_NAME__VELOCITY_I              "velocity_i"
#define PARAM_NAME__VELOCITY_D              "velocity_d"
#define PARAM_NAME__VELOCITY_OUTPUT_RAMP    "velocity_output_ramp"
#define PARAM_NAME__VELOCITY_LPF_TF         "velocity_lpf_tf"

#define PARAM_NAME__ANGLE_P             "angle_p"
#define PARAM_NAME__ANGLE_I             "angle_i"
#define PARAM_NAME__ANGLE_D             "angle_d"
#define PARAM_NAME__ANGLE_OUTPUT_RAMP   "angle_output_ramp"
#define PARAM_NAME__ANGLE_LPF_TF        "angle_lpf_tf"

#define PARAM_NAME__DRIVER_VOLTAGE_LIMIT    "motor_driver_voltage_limit"
#define PARAM_NAME__DRIVER_CURRENT_LIMIT    "driver_current_limit"
#define PARAM_NAME__MOTOR_VOLTAGE_LIMIT     "motor_voltage_limit"
#define PARAM_NAME__MOTOR_CURRENT_LIMIT     "motor_current_limit"
#define PARAM_NAME__MOTOR_VELOCITY_LIMIT    "motor_velocity_limit"

#define NUM_PARAMETERS 20

struct parameter_base_t {
    const char* key;
    rclc_parameter_type_t type;

    parameter_base_t(const char* k) : key(k) {}

    virtual ~parameter_base_t() {}
};

struct parameter_bool_t : parameter_base_t {
    bool value;

    parameter_bool_t(const char* k) : parameter_base_t(k), value(0) {}
};

struct parameter_int_t : parameter_base_t {
    int32_t value;

    parameter_int_t(const char* k) : parameter_base_t(k), value(0) {}
};

struct parameter_double_t : parameter_base_t {
    double_t value;

    parameter_double_t(const char* k) : parameter_base_t(k), value(0) {}
};

extern parameter_int_t controllerMode;
extern parameter_int_t polePairs;
extern parameter_double_t phaseResistance;
extern parameter_int_t kvRating;
extern parameter_double_t phaseInductance;
extern parameter_double_t velocityP;
extern parameter_double_t velocityI;
extern parameter_double_t velocityD;
extern parameter_double_t velocityOutputRamp;
extern parameter_double_t velocityLPFTf;
extern parameter_double_t angleP;
extern parameter_double_t angleI;
extern parameter_double_t angleD;
extern parameter_double_t angleOutputRamp;
extern parameter_double_t angleLPFTf;
extern parameter_double_t driverVoltageLimit;
extern parameter_double_t driverCurrentLimit;
extern parameter_double_t motorVoltageLimit;
extern parameter_double_t motorCurrentLimit;
extern parameter_double_t motorVelocityLimit;
extern parameter_base_t* params[NUM_PARAMETERS];


// Initialize parameter server options
const rclc_parameter_options_t parameterServiceOpts = {
    .notify_changed_over_dds = true,        // Publish parameter events to other ROS 2 nodes as well.
    .max_params = NUM_PARAMETERS,           // Maximum number of parameters allowed on the rclc_parameter_server_t object.
    .allow_undeclared_parameters = false,   // Allows creation of parameters from external parameter clients.
    .low_mem_mode = false                   // Reduces the memory used by the parameter server, applies constraints
};

void initializeParameterService();
void addBooleanParameter(const char* paramName, bool paramValue);
void addIntegerParameter();
void addDoubleParameter();

// =======================
// === SERVICE OBJECTS ===
// =======================


extern rcl_service_t setControllerModeService;
extern rcl_service_t deviceIdentifyService;
extern rcl_service_t setTargetService;
extern rcl_service_t enableMotorService;
extern rcl_service_t disableMotorService;
extern rclc_parameter_server_t parameterService;
extern SetControllerMode_Request setControllerModeRequest;
extern SetControllerMode_Response setControllerModeResponse;
extern Trigger_Request deviceIdentifyRequest;
extern Trigger_Response deviceIdentifyResponse;
extern SetTarget_Request setTargetRequest;
extern SetTarget_Response setTargetResponse;
extern Trigger_Request enableMotorRequest;
extern Trigger_Request enableMotorResponse;
extern Trigger_Request disableMotorRequest;
extern Trigger_Request disableMotorResponse;


// =========================
// === SERVICE CALLBACKS ===
// =========================


void setControllerModeCallback(const void* req, void* res);
void deviceIdentifyCallback(const void* req, void* res);
void setTargetCallback(const void* req, void* res);
void enableMotorCallback(const void* req, void* res);
void disableMotorCallback(const void* req, void* res);
bool onParameterChangedCallback(const Parameter* oldParam, const Parameter* newParam, void* context);

#endif // MICRO_ROS__SERVICES_H