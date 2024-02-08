#ifndef micro_ros_bldc_h
#define micro_ros_bldc_h

#pragma once
#include "micro_ros__actions.h"
#include "micro_ros__pubsub.h"
#include "micro_ros__services.h"
#include "micro_ros__parameters.h"
#include "neopixel.h"


// =======================
// === TIMER CALLBACKS ===
// =======================


extern TaskHandle_t MicroROSTask;

void microROSTaskCallback(void *parameter);
void neopixelTimerCallback(rcl_timer_t * timer, int64_t last_call_time);


// ======================
// === NODE FUNCTIONS ===
// ======================


bool createPublishers();
bool createServices();
bool createServicers();
bool addServices();
bool createTimers();
bool addTimers();
bool createEntities();
void destroyEntities();
void microROSNodeSetup();

#endif // micro_ros_bldc_h