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
float target = 5.0;
bool direction = true; // Motor direction. FALSE - counter-clockwise; TRUE - clockwise
TaskHandle_t Task1;

void controlMotorTask( void * parameter) {
    Serial.printf("Motor control on Core %d\r\n", xPortGetCoreID());
    for(;;) {
        motor.loopFOC();
        motor.move(target);
    }
}

void focBLDCSetup() {
    Wire.begin();
    // use monitoring with serial 
    Serial.begin(115200);

    // initialize magnetic sensor hardware
    Serial.println("Initializing FOC sensor...");
    sensorFOC.init();
    // link the motor to the sensor
    motor.linkSensor(&sensorFOC);

    // driver config
    Serial.println("Initializing driver...");
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
    motor.LPF_velocity.Tf = 0.01;

    // since the phase resistance is provided we set the current limit not voltage
    // default 0.2
    motor.current_limit = 1; // Amps

    // comment out if not needed
    motor.useMonitoring(Serial);

    // initialize motor
    motor.init();
    // align sensor and start FOC
    motor.initFOC();

    Serial.println("Motor ready.");
    _delay(1000);

    // xTaskCreatePinnedToCore(
    //     controlMotorTask, /* Function to implement the task */
    //     "Motor Control", /* Name of the task */
    //     10000,  /* Stack size in words */
    //     NULL,  /* Task input parameter */
    //     0,  /* Priority of the task */
    //     &Task1,  /* Task handle. */
    //     0); /* Core where the task should run */

    Serial.print("setup() running on core ");
    Serial.println(xPortGetCoreID());
}

#endif // FOC_BLDC_H