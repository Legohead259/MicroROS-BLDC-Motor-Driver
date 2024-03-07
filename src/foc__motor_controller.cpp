#include "foc__motor_controller.h"

bool MotorController::initialize_default() {
    return initialize(&defaultSettings);
}

bool MotorController::initialize(const MotorControllerSettings* settings) {
    setSettings(settings);
    focGenericSensor.init();
    motor.linkSensor(&focGenericSensor);
    driver.init();
    motor.linkDriver(&driver);
    if (flags.useMonitoring) motor.useMonitoring(Serial1);
    motor.init();
    if (!currentSensor.init()) { return false; }
    currentSensor.linkDriver(&driver);
    motor.linkCurrentSense(&currentSensor);
    motor.initFOC();
    disable();
    return true;
}

void MotorController::run() {
    focSensorMutex->lock(); // Block execution until focSensor is released, then grab it
    motorMutex->lock(); // Block execution until motor is released, then grab it
    motor.loopFOC();
    motor.move(target);
    focSensorMutex->unlock(); // Release focSensor
    motorMutex->unlock(); // Release motor
    delay(5); // Open up some time for other threads to use the sensor and motor
}

void MotorController::focSensorUpdate() {
    focSensorMutex->lock();
    focGenericSensor.update();
    focSensorMutex->unlock();
}

bool MotorController::getEnabled() {
    motorMutex->lock();
    bool _isEnabled = motor.enabled;
    motorMutex->unlock();
    return _isEnabled;
}

float MotorController::getShaftAngle() {
    focSensorMutex->lock();
    float _angle = motor.shaftAngle();
    focSensorMutex->unlock();
    return _angle;
}

float MotorController::getShaftVelocity() {
    focSensorMutex->lock();
    float _velocity = motor.shaftVelocity();
    focSensorMutex->unlock();
    return _velocity;
}

void MotorController::setSettings(const MotorControllerSettings* settings) {
    Serial.println("Set Settings");
    motor.controller = (MotionControlType) settings->controllerMode;
    motor.pole_pairs = settings->polePairs;
    motor.phase_resistance = settings->phaseResistance;
    motor.KV_rating = settings->kvRating;
    motor.phase_inductance = settings->phaseInductance;
    motor.PID_velocity.P = settings->velocityP;
    motor.PID_velocity.I = settings->velocityI;
    motor.PID_velocity.D = settings->velocityD;
    motor.PID_velocity.output_ramp = settings->velocityOutputRamp;
    motor.LPF_velocity.Tf = settings->velocityTF;
    motor.P_angle.P = settings->angleP;
    motor.P_angle.I = settings->angleI;
    motor.P_angle.D = settings->angleD;
    motor.P_angle.output_ramp = settings->angleOutputRamp;
    motor.LPF_angle.Tf = settings->angleTF;
}

void MotorController::setTarget(float _target) {
    target = _target;
}

void MotorController::reverse() {
    target = -target;
}

void MotorController::disable() {
    motor.disable();
}

void MotorController::enable() {
    motor.enable();
}