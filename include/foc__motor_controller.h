#ifndef FOC__MOTOR_CONTROLLER_H
#define FOC__MOTOR_CONTROLLER_H

#include "foc__angle_sensor.h"

#include <SimpleFOC.h>
#include <Wire.h>
#include <mutex>

void initTMAG5273Callback();
float readTMAG5273Callback();

extern TaskHandle_t focTask;

// Based off xio-Technologies Fusion structure layout

typedef struct {
    int controllerMode;
    int polePairs;
    float phaseResistance;
    int kvRating;
    float phaseInductance;
    float shuntResistance;
    float gain;

    float velocityP;
    float velocityI;
    float velocityD;
    float velocityOutputRamp;
    float velocityTF;

    float angleP;
    float angleI;
    float angleD;
    float angleOutputRamp;
    float angleTF;

    float driverVoltageLimit;
    long driverPWMFrequency;
    float motorVoltageLimit;
    float motorCurrentLimit;
    float motorVelocityLimit;
} MotorControllerSettings;

typedef struct {
    // TODO: Populate with internal states???
} MotorControllerInternalStates;

typedef struct MotorControllerFlags {
    bool useMonitoring;
    bool controllerInitialized;
    bool angleSensorInitialized;
    bool currentSensorInitialized;
} MotorControllerFlags;

struct MotorController {   
    MotorController(BLDCMotor& m, BLDCDriver& d, CurrentSense& ics, GenericSensor& gs)
        : motor(m),
        driver(d),
        currentSensor(ics),
        focGenericSensor(gs) {
            focSensorMutex = new std::mutex();
            motorMutex = new std::mutex();
        };
    
    MotorControllerFlags flags;
    MotorControllerSettings settings;
    BLDCMotor& motor;
    BLDCDriver& driver;
    CurrentSense& currentSensor;
    GenericSensor& focGenericSensor;

    float target;

    bool initialize_default();
    bool initialize(const MotorControllerSettings* settings);
    
    void run();
    void focSensorUpdate();

    bool getEnabled();
    float getShaftAngle();
    float getShaftVelocity();

    void setSettings(const MotorControllerSettings* settings);
    void setTarget(float _target);
    void reverse();
    void enable();
    void disable();

    std::mutex* focSensorMutex; ///< Protects access to the controller's FOC sensor. This is required because the FOC control loop and the other applications may ping the sensor simultaneously.
    std::mutex* motorMutex; ///<Protects access to the motor object. This is required because the FOC control loop and the other applications may ping the motor simultaneously.
};

const MotorControllerSettings defaultSettings = {
    .controllerMode = MotionControlType::velocity_openloop,
    .polePairs = 7,
    .phaseResistance = 3.0,
    .kvRating = 600,
    .phaseInductance = 3.0,
    .velocityP = DEF_PID_VEL_P,
    .velocityI = DEF_PID_VEL_I,
    .velocityD = DEF_PID_VEL_D,
    .velocityOutputRamp = DEF_PID_VEL_RAMP,
    .velocityTF = DEF_VEL_FILTER_Tf,
    .angleP = DEF_P_ANGLE_P,
    .angleI = 0,
    .angleD = 0,
    .angleOutputRamp = 1e6,
    .angleTF = 0,
    .driverVoltageLimit = 3.3,
    .driverPWMFrequency = 20000,
    .motorVoltageLimit = 3.3,
    .motorCurrentLimit = DEF_CURRENT_LIM,
    .motorVelocityLimit = DEF_VEL_LIM
};

void controlMotorTask( void * parameter);

void focBLDCSetup();

#endif // FOC__MOTOR_CONTROLLER_H