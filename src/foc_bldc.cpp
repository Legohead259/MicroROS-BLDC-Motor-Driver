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
    Serial1.print("Starting TMAG5273...");
    if(!sensor.begin(TMAG5273_I2C_ADDRESS_INITIAL)) { 
        Serial1.println("Failed to start TMAG5273");
        while(1); // Stop further code execution
    }
    sensor.setConvAvg(TMAG5273_X32_CONVERSION);
    sensor.setAngleEn(0x01);
    angleSensorInitialized = true;
    Serial1.println("Done!");
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