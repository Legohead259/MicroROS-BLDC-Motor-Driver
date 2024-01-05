#include "micro_ros__services.h"


// =======================
// === SERVICE OBJECTS ===
// =======================


rcl_service_t setControllerModeService;
rcl_service_t deviceIdentifyService;
rcl_service_t setTargetService;
rcl_service_t enableMotorService;
rcl_service_t disableMotorService;
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
// === SERVICE CALLBACKS ===
// =========================


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