#include "foc__motor_controller.h"

bool MotorController::initialize() {
    const MotorControllerSettings settings = {

    };
    setSettings(&settings);
    focGenericSensor.init();
    motor.linkSensor(&focGenericSensor);
    driver.init();
    motor.linkDriver(&driver);
    if (flags.useMonitoring) motor.useMonitoring(Serial);
    motor.init();
    if (!currentSensor.init()) { return false; }
    currentSensor.linkDriver(&driver);
    motor.linkCurrentSense(&currentSensor);
    motor.initFOC();
    disable();
    return true;
}

void MotorController::setSettings(const MotorControllerSettings* settings) {
    Serial.println("Set Settings");
    motor.controller = (MotionControlType) settings->controllerMode;
}

void MotorController::run() {
    focSensorMutex->lock(); // Block execution until focSensor is released, then grab it
    motor.loopFOC();
    motor.move(target);
    focSensorMutex->unlock(); // Release focSensor
}

void MotorController::setTarget(float _target) {
    target = _target;
}

void MotorController::disable() {
    motor.disable();
}

void MotorController::enable() {
    motor.enable();
}

GenericSensor sensorFOC = GenericSensor(readTMAG5273Callback, initTMAG5273Callback);
InlineCurrentSense currentSensor = InlineCurrentSense(0.012, 20, PHASE_U_SENSE, PHASE_V_SENSE, PHASE_W_SENSE);
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(
                            PHASE_U_HIGH, PHASE_U_LOW, 
                            PHASE_V_HIGH, PHASE_V_LOW, 
                            PHASE_W_HIGH, PHASE_W_LOW, 
                            DRIVER_ENABLE);