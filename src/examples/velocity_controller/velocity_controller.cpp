#include <SimpleFOC.h>
#include <SparkFun_TMAG5273_Arduino_Library.h>
#include <Arduino.h>

void initTMAG5273Callback();
float readTMAG5273Callback();

TMAG5273 sensor;
GenericSensor sensorFOC = GenericSensor(readTMAG5273Callback, initTMAG5273Callback);
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(
                            PHASE_U_HIGH, PHASE_U_LOW, 
                            PHASE_V_HIGH, PHASE_V_LOW, 
                            PHASE_W_HIGH, PHASE_W_LOW, 
                            DRIVER_ENABLE);

float target = 0.0;

void initTMAG5273Callback() {
    sensor.setAngleEn(false);
    if (!sensor.begin(TMAG5273_I2C_ADDRESS_INITIAL)) {
        Serial.println("Sensor failed to init...");
        while(1);
    }
    sensor.setAngleEn(true);
}
float readTMAG5273Callback() {
    return sensor.getAngleResult() / 180 * PI;
}

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
    driver.voltage_power_supply = 3.3;
    driver.pwm_frequency = 20000;
    driver.voltage_limit = 4;
    driver.init();
    motor.linkDriver(&driver);

    // set motion control loop to be used
    motor.voltage_limit = 4.0;
    motor.KV_rating = 600;
    motor.controller = MotionControlType::velocity;

    // controller configuration 
    // default parameters in defaults.h

    // controller configuration based on the control type 
    // velocity PID controller parameters
    // default P=0.5 I = 10 D =0
    motor.PID_velocity.P = 0.1;
    motor.PID_velocity.I = 1.0;
    motor.PID_velocity.D = 0.0;
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
float target_velocity = 50; // 2Rad/s ~ 20rpm

void loop() {
    // main FOC algorithm function
    motor.loopFOC();

    // Motion control function
    motor.move(target_velocity);
}