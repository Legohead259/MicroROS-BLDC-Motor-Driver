#ifndef FOC_BLDC_H
#define FOC_BLDC_H

#include <SimpleFOC.h>
#include <SparkFun_TMAG5273_Arduino_Library.h>
#include <Wire.h>

#define uh16      16
#define ul17      17
#define vh18      18
#define wh19      19
#define vl23      23
#define wl33      33
#define curSense  32


TMAG5273 sensor;
bool angleSensorInitialized = false;
bool currentSensorInitialized = false;

void initTMAG5273Callback(){
    if(!sensor.begin(TMAG5273_I2C_ADDRESS_INITIAL)) { 
        while(1); // Stop further code execution
    }
    sensor.setAngleEn(0x01);
    angleSensorInitialized = true;
}

float readTMAG5273Callback(){
    return sensor.getAngleResult() / 180 * PI;
}

GenericSensor sensorFOC = GenericSensor(readTMAG5273Callback, initTMAG5273Callback);
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(uh16, ul17, vh18, vl23, wh19, wl33,  curSense);
float target = 0.0;
bool direction = true; // Motor direction. FALSE - counter-clockwise; TRUE - clockwise
TaskHandle_t Task1;

void controlMotorTask( void * parameter) {
    for(;;) {
        motor.loopFOC();
        motor.move(target);
    }
}

void focBLDCSetup() {
    Wire.begin();
    
    driver.voltage_power_supply = 3.3;
    driver.pwm_frequency = 20000;
    driver.voltage_limit = 4;
    driver.init();

    sensorFOC.init();
    motor.linkSensor(&sensorFOC);

    motor.linkDriver(&driver);
    motor.voltage_limit = 4;
    motor.controller = MotionControlType::velocity;
    motor.init();
    motor.initFOC();

    xTaskCreatePinnedToCore(
        controlMotorTask, /* Function to implement the task */
        "Motor Control", /* Name of the task */
        10000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &Task1,  /* Task handle. */
        0); /* Core where the task should run */
}

#endif // FOC_BLDC_H