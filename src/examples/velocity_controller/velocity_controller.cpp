#include <SimpleFOC.h>
#include "foc_bldc.h"
#include <Arduino.h>

void setup() {
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
}

// velocity set point variable
float target_velocity = 10; // 2Rad/s ~ 20rpm

void loop() {
    // main FOC algorithm function
    motor.loopFOC();

    // Motion control function
    motor.move(target_velocity);
}