/**
 * Find KV rating for motor with magnetic sensors
 *
 * Motor KV rating is defined as the increase of the motor velocity expressed in rotations per minute [rpm] per each 1 Volt int voltage control mode.
 * 
 * This example will set your motor in the torque control mode using voltage and set 1 volt to the motor. By reading the velocity it will calculate the motors KV rating.
 * - To make this estimation more credible you can try increasing the target voltage (or decrease in some cases) 
 * - The KV rating should be relatively static number - it should not change considerably with the increase in the voltage
 */

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
    sensor.setAngleEn(false);
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

// voltage set point variable
float targetVoltage = 1;

// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&targetVoltage, cmd); }
void calcKV(char* cmd) { 
    // calculate the KV
    Serial.println(motor.shaft_velocity/motor.target*30.0f/_PI);

}

void setup() { 
    // use monitoring with serial 
    Serial.begin(115200);
    Serial.println("Starting...");
    Wire.begin();

    // comment out if not needed
    motor.useMonitoring(Serial);

    // initialize encoder sensor hardware
    Serial.println("Initializing FOC sensor...");
    sensorFOC.init();
    // link the motor to the sensor
    Serial.println("Linking FOC sensor...");
    motor.linkSensor(&sensorFOC);

    // driver config
    // IMPORTANT!
    // make sure to set the correct power supply voltage [V]
    driver.voltage_power_supply = 3.3;
    driver.init();
    // link driver
    motor.linkDriver(&driver);

    // aligning voltage
    motor.voltage_sensor_align = 3;

    // set motion control loop to be used
    motor.controller = MotionControlType::torque;

    // initialize motor
    motor.init();
    // align sensor and start FOC
    motor.initFOC();

    // add target command T
    command.add('T', doTarget, "target voltage");
    command.add('K', calcKV, "calculate KV rating");

    Serial.println(F("Motor ready."));
    Serial.println(F("Set the target voltage : - command T"));
    Serial.println(F("Calculate the motor KV : - command K"));
    _delay(1000);

    Serial.println("Timestamp (ms) | Loop time (ms) | Shaft velocity (rps)");
}


void loop() {
    // static uint32_t _lastCall = 0;
    // main FOC algorithm function
    // the faster you run this function the better
    // Arduino UNO loop  ~1kHz
    // Bluepill loop ~10kHz 
    motor.loopFOC();

    // Motion control function
    // velocity, position or voltage (defined in motor.controller)
    // this function can be run at much lower frequency than loopFOC() function
    // You can also use motor.move() and set the motor.target in the code
    motor.move(targetVoltage);

    // user communication
    command.run();

    // Serial.printf("%lu | %lu | %d\r\n", millis(), millis()-_lastCall, motor.shaft_velocity);
    // _lastCall = millis();

    // Serial.println(sensor.getAngleResult());
}