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
void attachMotorController(MotorController* mc);

#endif // micro_ros_bldc_h