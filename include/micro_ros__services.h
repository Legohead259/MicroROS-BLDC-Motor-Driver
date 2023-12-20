#ifndef MICRO_ROS__SERVICES_H
#define MICRO_ROS__SERVICES_H

#include "micro_ros__definitions.h"


// =======================
// === SERVICE OBJECTS ===
// =======================


extern rcl_service_t setControllerModeService;
extern rcl_service_t setMotorDirectionService;
extern rcl_service_t setTargetService;
extern SetControllerMode_Request setControllerModeRequest;
extern SetControllerMode_Response setControllerModeResponse;
extern SetMotorDirection_Request setMotorDirectionRequest;
extern SetMotorDirection_Response setMotorDirectionResponse;
extern SetTarget_Request setTargetRequest;
extern SetTarget_Response setTargetResponse;


// =========================
// === SERVICE CALLBACKS ===
// =========================


void setControllerModeCallback(const void* req, void* res);
void setMotorDirectionCallback(const void* req, void* res);
void setTargetCallback(const void* req, void* res);

#endif // MICRO_ROS__SERVICES_H