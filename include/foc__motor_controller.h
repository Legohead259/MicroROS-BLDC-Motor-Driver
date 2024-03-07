#ifndef FOC__MOTOR_CONTROLLER_H
#define FOC__MOTOR_CONTROLLER_H

#include "foc__angle_sensor.h"

#include <SimpleFOC.h>
#include <Wire.h>
#include <mutex>

extern std::mutex sensorFOCMutex;
extern InlineCurrentSense currentSensor;
extern BLDCMotor motor;
extern BLDCDriver6PWM driver;
extern GenericSensor sensorFOC;

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
        bldcDriver(d),
        currentSensor(ics),
        focGenericSensor(gs) {
            focSensorMutex = new std::mutex();
        };
    
    MotorControllerFlags flags;
    MotorControllerSettings settings;
    BLDCMotor& motor;
    BLDCDriver& bldcDriver;
    CurrentSense& currentSensor;
    GenericSensor& focGenericSensor;

    float target;

    bool initialize();
    void setSettings(const MotorControllerSettings* settings);
    
    void run();

    void setTarget(float _target);
    void enable();
    void disable();

    std::mutex* focSensorMutex; ///< Mutex to protect access to the controller's FOC sensor. This is required because the FOC control loop and the other applications may ping the sensor simultaneously.
};

void controlMotorTask( void * parameter);

void focBLDCSetup();

#endif // FOC__MOTOR_CONTROLLER_H