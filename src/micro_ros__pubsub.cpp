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
rcl_timer_t angularPositionTimer;
rcl_timer_t motorDirectionTimer;
AngularAccelerationMsg angularAccelerationMsg;
AngularVelocityMsg angularVelocityMsg;
AngularPositionMsg angularPositionMsg;
MotorStatusMsg motorDirectionMsg;


// ===========================
// === PUBLISHER CALLBACKS ===
// ===========================


void angularPositionCallback(rcl_timer_t * timer, int64_t last_call_time) {
    RCLC_UNUSED(last_call_time);
    if (timer != NULL) {
        RCSOFTCHECK(rcl_publish(&angularPositionPublisher, &angularPositionMsg, NULL));
        angularPositionMsg.timestamp = millis();
        angularPositionMsg.angular_position = sensor.getAngleEn() != 0 ? sensor.getAngleResult() : -999;
    }
}