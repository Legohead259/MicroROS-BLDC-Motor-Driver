#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

// Enum to represent different states
enum State {
    IDLE_NO_CONNECTION,
    IDLE_WITH_CONNECTION,
    PARTIAL_FORWARD,
    PARTIAL_REVERSE,
    FULL_FORWARD,
    FULL_REVERSE,
    DEVICE_IDENTIFY,
    MOTOR_DISABLED,
    SENSOR_FAULT,
    DRIVER_FAULT,
    TRANSPORT_FAULT
};

extern State systemState;
extern State previousSystemState;

class StateMachine {
public:
    StateMachine(State &initialState) : currentState(initialState) {}

    // Function to perform actions based on the current state
    virtual void executeState() {};

protected:
    // Variable to store the current state
    State &currentState;
};

static void changeSystemState(State newState) {
    previousSystemState = systemState;
    systemState = newState;
}

#endif // STATE_MACHINE_H
