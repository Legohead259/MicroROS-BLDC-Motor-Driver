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
#include "micro_ros_bldc.h"
#include "foc.h"
#include "neopixel.h"

GenericSensor sensorFOC = GenericSensor(readTMAG5273Callback, initTMAG5273Callback);
InlineCurrentSense currentSensor = InlineCurrentSense(0.012, 20, PHASE_U_SENSE, PHASE_V_SENSE, PHASE_W_SENSE);
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(
                            PHASE_U_HIGH, PHASE_U_LOW, 
                            PHASE_V_HIGH, PHASE_V_LOW, 
                            PHASE_W_HIGH, PHASE_W_LOW, 
                            DRIVER_ENABLE);
MotorController motorController = MotorController(motor, driver, currentSensor, sensorFOC);
TaskHandle_t focTask;

void controlMotorTask( void * parameter) {
    for(;;) {
        motorController.run();
    }
}

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
 * @param parameters
 */
void microROSTaskCallback(void* parameters) {
    for(;;) {
        
        // Handle system LED
        ledStateMachine.executeState();

        // Handle Micro-ROS tasking
        switch (agentState) {
            case WAITING_AGENT:
                EXECUTE_EVERY_N_MS(500, agentState = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_AVAILABLE : WAITING_AGENT;);
                break;

            case AGENT_AVAILABLE:
                agentState = createEntities() ? AGENT_CONNECTED : WAITING_AGENT; // Check if entities are properly created
                if (agentState == AGENT_CONNECTED) { // Update system state
                    StateMachine::changeSystemState(IDLE_WITH_CONNECTION);
                    motorController.enable();
                }
                if (agentState == WAITING_AGENT) { // If entities are not properly created, destroy them
                    destroyEntities();
                    motorController.disable();
                };
                break;
            
            case AGENT_CONNECTED:
                EXECUTE_EVERY_N_MS(200, agentState = (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) ? AGENT_CONNECTED : AGENT_DISCONNECTED;);
                if (agentState == AGENT_CONNECTED) {
                    // Execute pending tasks in the executor. This will handle all ROS communications.
                    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
                }
                break;

            case AGENT_DISCONNECTED:
                destroyEntities();
                agentState = WAITING_AGENT;
                StateMachine::changeSystemState(IDLE_NO_CONNECTION);
                motorController.disable();
                break;
                
            default:
                break;
        }
    }
}

/**
 * This runs before `loop()` on MCU start
*/
void setup() {
    Wire.begin();
    Serial.begin(500000);
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);

    ledStateMachine.begin();

    motorController.initialize_default();
    
    xTaskCreatePinnedToCore(
        controlMotorTask,   // Function to implement the task 
        "Motor Control",    // Name of the task
        10000,              // Stack size in words
        NULL,               // Task input parameter
        1,                  // Priority of the task
        &focTask,           // Task handle.
        1);                 // Core where the task should run

    attachMotorController(&motorController);

    xTaskCreatePinnedToCore(
        microROSTaskCallback, /* Function to implement the task */
        "microROS", /* Name of the task */
        25000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &MicroROSTask,  /* Task handle. */
        0); /* Core where the task should run */

    set_microros_serial_transports(Serial); // Configure Micro-ROS library to use Arduino serial
}

/**
 * This runs continually
*/
void loop() {

}
