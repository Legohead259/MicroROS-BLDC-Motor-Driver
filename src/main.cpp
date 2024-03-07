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

#include <Wire.h>
#include <Arduino.h>
// #include "micro_ros.h"
#include "foc.h"
// #include "neopixel.h"

MotorController motorController = MotorController(motor, driver, currentSensor, sensorFOC);
TaskHandle_t focTask;

void controlMotorTask( void * parameter) {
    for(;;) {
        motorController.run();
    }
}
/**
 * This runs before `loop()` on MCU start
*/
void setup() {
    Wire.begin();
    Serial.begin(115200); // TODO: Set back to 500000 or greater for Micro-ROS functions
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);

    Serial1.print("Starting LED state machine...");
    // ledStateMachine.begin();
    Serial1.println("Done!");

    // TODO: REMEMBER TO REINCLUDE MICRO_ROS SOURCE FILES IN THE SRC FILTER
    // microROSNodeSetup();
    // focBLDCSetup();

    motorController.initialize();

    xTaskCreatePinnedToCore(
        controlMotorTask,   // Function to implement the task 
        "Motor Control",    // Name of the task
        10000,              // Stack size in words
        NULL,               // Task input parameter
        1,                  // Priority of the task
        &focTask,           // Task handle.
        1);                 // Core where the task should run

    delay(2000);
    motorController.enable();
    motorController.setTarget(5.0);
    delay(2000);
    motorController.disable();
}

/**
 * This runs continually
*/
void loop() {

}
