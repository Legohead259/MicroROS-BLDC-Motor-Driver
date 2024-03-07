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

    StateMachine::changeSystemState(DEVICE_IDENTIFY);

    // Send response back to client
    res_in->success = true;
}

void setTargetCallback(const void* req, void* res) {
    SetTarget_Request* req_in = (SetTarget_Request*) req;
    SetTarget_Response* res_in = (SetTarget_Response*) res;

    // Set target velocity based on request value
    motorControllerPtr->setTarget(req_in->target);

    // Update system state from target
    if (req_in->target != 0) {
        StateMachine::changeSystemState((req_in->target > 0) ? PARTIAL_FORWARD : PARTIAL_REVERSE);
    }
    // TODO: Set and check maxes (FULL_FORWARD, FULL_REVERSE)
    else {
        StateMachine::changeSystemState(previousSystemState);
    }

    // Send response back to client
    res_in->result = true;
}

void enableMotorCallback(const void* req, void* res) {
    Trigger_Request* req_in = (Trigger_Request*) req;
    Trigger_Response* res_in = (Trigger_Response*) res;

    motorControllerPtr->enable();
    res_in->success = motorControllerPtr->getEnabled();
}

void disableMotorCallback(const void* req, void* res) {
    Trigger_Request* req_in = (Trigger_Request*) req;
    Trigger_Response* res_in = (Trigger_Response*) res;

    motorControllerPtr->disable();
    res_in->success = !motorControllerPtr->getEnabled();
}