/**
 * @file micro_ros_bldc.h
 * @brief The main header file for the Micro-ROS node running on the ESC.
 * Contains the sub-includes and specifies some of the callbacks and main node
 * functions.
 * 
 * @author Braidan Duffy
 * @copyright 2024
 * 
 * @version 1.0.0
 */

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


// The FreeRTOS task to execute Micro-ROS on a core
extern TaskHandle_t MicroROSTask;

/**
 * @brief The callback that is executed by the `MicroROSTask` every cycle
 * Creates an infinite loop where the LED state machine is handled based on the Micro-ROS agent status
 * When the Micro-ROS agent is unconnected, the system will continually attempt to connect to the agent
 * over the transport layer.
 * This reconnection is attempted every 500 ms with a timeout of 100ms.
 * 
 * When the agent is connected, all of the Micro-ROS entities are created using `createEntities()`.
 * When the entities are created, the system state is changed to `AGENT_CONNECTED` and the motor is enabled
 * and the ESC should be connected to the host ROS2 system.
 * 
 * 
 * @param parameter 
 */
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