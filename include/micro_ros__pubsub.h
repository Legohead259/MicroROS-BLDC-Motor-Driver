#ifndef MICRO_ROS__PUBSUB_H
#define MICRO_ROS__PUBSUB_H

#include "micro_ros__definitions.h"


// =======================
// === PUB/SUB OBJECTS ===
// =======================


extern rcl_publisher_t angularAccelerationPublisher;
extern rcl_publisher_t angularVelocityPublisher;
extern rcl_publisher_t angularPositionPublisher;
extern rcl_publisher_t motorDirectionPublisher;
extern rcl_timer_t angularAccelerationTimer;
extern rcl_timer_t angularVelocityTimer;
extern rcl_timer_t angularMeasurementTimer;
extern rcl_timer_t motorDirectionTimer;
extern AngularMeasurementMsg angularMeasurementMsg;
extern MotorStatusMsg motorDirectionMsg;


// ===========================
// === PUBLISHER CALLBACKS ===
// ===========================


void angularMeasurementCallback(rcl_timer_t * timer, int64_t last_call_time);

#endif // MICRO_ROS__PUBSUB_H