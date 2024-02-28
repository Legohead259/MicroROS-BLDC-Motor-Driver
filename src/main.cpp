/**
 * @file main.cpp
 * @brief The main execution for the OlympianESC.
 * The `setup()` function initializes the Serial communication ports, state machine,
 * Micro-ROS node, and SimpleFOC controller.
 * 
 * @author Braidan Duffy
 * @copyright 2024
 * 
 * @version 1.0.0
 */

#include "micro_ros_bldc.h"
#include "foc_bldc.h"
#include "neopixel.h"

/**
 * This runs before `loop()` on MCU start
*/
void setup() {
    Wire.begin();
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);

    Serial1.print("Starting LED state machine...");
    ledStateMachine.begin();
    Serial1.println("Done!");

    microROSNodeSetup();
    focBLDCSetup();
}

/**
 * This runs continually
*/
void loop() {

}
