#include "foc_bldc.h"


// ===================
// === FOC OBJECTS ===
// ===================


std::mutex sensorFOCMutex;
TMAG5273 sensor;
bool angleSensorInitialized = false;
InlineCurrentSense currentSensor = InlineCurrentSense(0.012, 20, PHASE_U_SENSE, PHASE_V_SENSE, PHASE_W_SENSE);
bool currentSensorInitialized = false;


// ================================
// === FOC SENSOR INSTANTIATION ===
// ================================


void initTMAG5273Callback() {
    return;
}

float readTMAG5273Callback() {
    return sensor.getAngleResult() / 180 * PI;
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
        sensorFOCMutex.lock(); // Block execution until sensorFOC is released, then grab it
        motor.loopFOC();
        motor.move(target);
        sensorFOCMutex.unlock(); // Release sensorFOC
        delay(10);
    }
}

void focBLDCSetup() {
    // initialize magnetic sensor hardware
    Serial1.println("Initializing FOC sensor...");
    sensorFOC.init();
    // link the motor to the sensor
    motor.linkSensor(&sensorFOC);

    // driver config
    Serial1.print("Initializing driver...");
    driver.voltage_power_supply = 3.3;
    driver.pwm_frequency = 20000;
    driver.init();
    motor.linkDriver(&driver);
    Serial1.println("done!");

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

    // Current Sensor init
    Serial1.print("Initializing current sensor...");
    if (!currentSensor.init()) {
        Serial1.println("Current sense init failed!");
        while(1); // Block further code execution
    }
    currentSensor.linkDriver(&driver);
    motor.linkCurrentSense(&currentSensor);
    Serial1.println("done!");
    
    // align sensor and start FOC
    motor.initFOC();
    motor.disable();

    Serial1.println("Motor ready.");

    xTaskCreatePinnedToCore(
        controlMotorTask, /* Function to implement the task */
        "Motor Control", /* Name of the task */
        10000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        1,  /* Priority of the task */
        &focTask,  /* Task handle. */
        1); /* Core where the task should run */
}