#include "foc_bldc.h"


// ===================
// === FOC OBJECTS ===
// ===================


SemaphoreHandle_t xSensorFOCMutex = NULL;
std::mutex sensorFOCMutex;
TMAG5273 sensor;
bool angleSensorInitialized = false;
bool currentSensorInitialized = false;
uint32_t shaftReadTime = 0;
float shaftAngle = 0;
float shaftAngularVelocity = 0;


// ================================
// === FOC SENSOR INSTANTIATION ===
// ================================


void initTMAG5273Callback() {
    return;
}

float readTMAG5273Callback() {
    // return shaftAngle;
    Serial1.println("Trying to lock mutex in readTMAG5273Callback()");
    sensorFOCMutex.lock();
    Serial1.println("Locked mutex in readTMAG5273Callback()");
    float _angle = sensor.getAngleResult() / 180 * PI;
    Serial1.println("Unlocking mutex in readTMAG5273Callback()");
    sensorFOCMutex.unlock();
    Serial1.println("readTMAG5273Callback() finished!");
    return _angle;
}

GenericSensor sensorFOC = GenericSensor(readTMAG5273Callback, initTMAG5273Callback);


// ===========================
// === MOTOR INSTANTIATION ===
// ===========================


BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(
                            PHASE_U_HIGH, PHASE_U_LOW, 
                            PHASE_V_HIGH, PHASE_V_LOW, 
                            PHASE_W_HIGH, PHASE_W_LOW, 
                            DRIVER_ENABLE);

float target = 0.0;
TaskHandle_t focTask;

void controlMotorTask( void * parameter) {
    Serial1.printf("Motor control on Core %d\r\n", xPortGetCoreID());
    for(;;) {
        uint32_t _startMicros = micros();
        motor.loopFOC();
        // Serial1.printf("Time to finish loopFOC(): %lu us\r\n", micros() - _startMicros);
        motor.move(target);
    }
}

void focBLDCSetup() {
    // initialize magnetic sensor hardware
    Serial1.println("Initializing FOC sensor...");
    sensorFOC.init();
    // link the motor to the sensor
    motor.linkSensor(&sensorFOC);

    // driver config
    Serial1.println("Initializing driver...");
    driver.init();
    motor.linkDriver(&driver);

    // set motion control loop to be used
    motor.controller = MotionControlType::velocity;

    // controller configuration 
    // default parameters in defaults.h

    // controller configuration based on the control type 
    // velocity PID controller parameters
    // default P=0.5 I = 10 D =0
    motor.PID_velocity.P = 0.2;
    motor.PID_velocity.I = 20;
    motor.PID_velocity.D = 0.001;
    // jerk control using voltage voltage ramp
    // default value is 300 volts per sec  ~ 0.3V per millisecond
    motor.PID_velocity.output_ramp = 1000;

    // velocity low pass filtering
    // default 5ms - try different values to see what is the best. 
    // the lower the less filtered
    // motor.LPF_velocity.Tf = 0.01;

    // since the phase resistance is provided we set the current limit not voltage
    // default 0.2
    motor.current_limit = 1; // Amps

    // comment out if not needed
    motor.useMonitoring(Serial1);

    // initialize motor
    motor.init();
    // align sensor and start FOC
    motor.initFOC();

    Serial1.println("Motor ready.");

    xTaskCreatePinnedToCore(
        controlMotorTask, /* Function to implement the task */
        "Motor Control", /* Name of the task */
        10000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &focTask,  /* Task handle. */
        1); /* Core where the task should run */
}