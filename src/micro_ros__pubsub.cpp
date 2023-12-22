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
        // std::lock_guard<std::mutex> lock(sensorFOCMutex);
        Serial1.println("Trying to lock mutex in angularMeasurementCallback()");
        sensorFOCMutex.lock();
        Serial1.println("Locked mutex in angularMeasurementCallback()");
        RCSOFTCHECK(rcl_publish(&angularPositionPublisher, &angularMeasurementMsg, NULL));
        // angularMeasurementMsg.timestamp = shaftReadTime;
        // angularMeasurementMsg.angular_position = shaftAngle;
        // angularMeasurementMsg.angular_velocity = shaftAngularVelocity;

        // angularMeasurementMsg.timestamp = millis();
        // angularMeasurementMsg.angular_position = sensorFOC.getSensorAngle();
        // angularMeasurementMsg.angular_velocity = sensorFOC.getVelocity();

        angularMeasurementMsg.timestamp = millis();
        angularMeasurementMsg.angular_position = sensor.getAngleResult() / 180 * PI;
        // angularMeasurementMsg.angular_velocity = sensorFOC.getVelocity();
        
        Serial1.println("Unlocking mutex in angularMeasurementCallback()");
        sensorFOCMutex.unlock();
        Serial1.println("angularMeasurementCallback() finished!");
        
    }
}