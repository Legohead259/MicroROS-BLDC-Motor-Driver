#ifndef MICRO_ROS__SERVICES_H
#define MICRO_ROS__SERVICES_H

#include "micro_ros__definitions.h"

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