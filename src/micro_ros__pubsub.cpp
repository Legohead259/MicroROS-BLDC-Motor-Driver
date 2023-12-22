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
        angularMeasurementMsg.angular_position = sensorFOC.getMechanicalAngle();
        angularMeasurementMsg.angular_velocity = sensorFOC.getVelocity();
        sensorFOCMutex.unlock(); // Release sensorFOC
    }
}