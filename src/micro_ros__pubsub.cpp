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
        sensorFOCMutex.lock(); // Block execution until sensorFOC is released, then grab it
        RCSOFTCHECK(rcl_publish(&angularPositionPublisher, &angularMeasurementMsg, NULL));
        sensorFOC.update();
        angularMeasurementMsg.timestamp = millis();
        angularMeasurementMsg.angular_position = motor.shaftAngle();
        angularMeasurementMsg.angular_velocity = motor.shaftVelocity();
        sensorFOCMutex.unlock(); // Release sensorFOC
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
        motor.enabled ? motor.disable() : motor.enable(); // Enable or disable motor
        target = 5.0;
    }

    if (msg->buttons.data[BUTTON_A] && (millis() - _lastSwitch) > SWITCH_TIMEOUT) { // Check for switch direction button pressed and timeout passed
        // TODO: Make thread safe
        _lastSwitch = millis();
        target = -target;
    }
}