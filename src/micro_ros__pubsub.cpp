#include "micro_ros__pubsub.h"


// =======================
// === PUB/SUB OBJECTS ===
// =======================


rcl_publisher_t angularAccelerationPublisher;
rcl_publisher_t angularVelocityPublisher;
rcl_publisher_t angularPositionPublisher;
rcl_publisher_t motorDirectionPublisher;
rcl_timer_t angularAccelerationTimer;
rcl_timer_t angularVelocityTimer;
rcl_timer_t angularMeasurementTimer;
rcl_timer_t motorDirectionTimer;
AngularMeasurementMsg angularMeasurementMsg;
MotorStatusMsg motorDirectionMsg;
rcl_subscription_t joySubscriber;
JoystickMsg joystickMsg;

const unsigned long ENABLE_TIMEOUT = 500; // ms
const unsigned long SWITCH_TIMEOUT = 500; // ms

unsigned long _lastEnable = 0; // Time variable for the last time "enable" was called for software debounce
unsigned long _lastSwitch = 0; // Time variable for the last time "switch" was called for software debounce

// ===========================
// === PUBLISHER CALLBACKS ===
// ===========================


void angularMeasurementCallback(rcl_timer_t * timer, int64_t last_call_time) {
    RCLC_UNUSED(last_call_time);
    if (timer != NULL) {
        RCSOFTCHECK(rcl_publish(&angularPositionPublisher, &angularMeasurementMsg, NULL));
        motorControllerPtr->focSensorUpdate();
        angularMeasurementMsg.timestamp = millis();
        angularMeasurementMsg.angular_position = motorControllerPtr->getShaftAngle();
        angularMeasurementMsg.angular_velocity = motorControllerPtr->getShaftVelocity();
    }
}


// ============================
// === SUBSCRIBER CALLBACKS ===
// ============================


void joystickCallback(const void* msgin) {
    const JoystickMsg* msg = (const JoystickMsg*) msgin;

    if (msg->buttons.data[START] && (millis() - _lastEnable) > ENABLE_TIMEOUT) { // Check for enable button pressed and timeout passed
        _lastEnable = millis();
        // TODO: Make thread safe
        motorControllerPtr->getEnabled() ? motorControllerPtr->disable() : motorControllerPtr->enable(); // Change motor enabled state
        motorControllerPtr->setTarget(5.0);
    }

    if (msg->buttons.data[BUTTON_A] && (millis() - _lastSwitch) > SWITCH_TIMEOUT) { // Check for switch direction button pressed and timeout passed
        // TODO: Make thread safe
        _lastSwitch = millis();
        motorControllerPtr->reverse();
    }
}