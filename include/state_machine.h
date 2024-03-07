/**
 * @file state_machine.h
 * @author Braidan Duffy
 * @brief Template class for the internal state machine.
 * This class does not hold any inherent value except as a superclass for the LED state machine.
 * This file defines the system states and abstracts the underlying state tracking.
 * 
 * @section usage Usage
 * The `StateMachine` object tracks the value of `systemState` through a pointer reference.
 * By changing the value of `systemState`, the machine will automatically adjust the execution
 * process defined in `executeState()`. 
 * This is best done through the state method `StateMachine::changeSystemState()` as it will
 * automatically update the previous system state as well.
 *
 * @version 1.0
 * @date 2024-03-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

// Enum to represent different states
enum State {
    IDLE_NO_CONNECTION,     ///< Micro-ROS node is awaiting connection to Agent
    IDLE_WITH_CONNECTION,   ///< Micro-ROS node is connected to Agent
    PARTIAL_FORWARD,        ///< Motor is moving forward, but not at full speed
    PARTIAL_REVERSE,        ///< Motor is moving reverse, but not at full speed
    FULL_FORWARD,           ///< Motor is moving forward at full speed
    FULL_REVERSE,           ///< Motor is moving reverse at full speed
    DEVICE_IDENTIFY,        ///< Micro-ROS node is commanded to identify itself using the LED
    MOTOR_DISABLED,         ///< Motor is disabled due to motor fault
    SENSOR_FAULT,           ///< One or more sensors have encountered an error
    DRIVER_FAULT,           ///< The motor driver has encountered an error
    TRANSPORT_FAULT         ///< The Micro-ROS transport layer has encountered an error
};

extern State systemState;           ///< Current system state. Defaults to `IDLE_NO_CONNECTION`.
extern State previousSystemState;   ///< Previous system state. Defaults to `IDLE_NO_CONNECTION`.

/**
 * @brief 
 * 
 */
class StateMachine {
public:
    /**
     * @brief Construct a new State Machine object. 
     * 
     * @param currentStatePtr Pointer to the system state global variable
     */
    StateMachine(State &currentStatePtr) : currentState(currentStatePtr) {}

    /**
     * @brief Execute functions based on the current system state
     * 
     * @note Should be executed in an asynchronous loop
     */
    virtual void executeState() {};

    /**
     * @brief Changes the value of the global `systemState` variable.
     * Saves the system state to `previousSystemState` before making the change.
     * If the State Machine object is initialized with the `systemState` variable, 
     * this will automatically update its internal state value.
     * 
     * @param newState The new desired system state
     */
    static void changeSystemState(State newState) {
        previousSystemState = systemState;
        systemState = newState;
    }

protected:
    State &currentState; ///< Variable to store the pointer to the memory address of the current system state
};

#endif // STATE_MACHINE_H
