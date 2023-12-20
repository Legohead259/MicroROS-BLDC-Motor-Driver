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

extern TMAG5273 sensor;
extern bool angleSensorInitialized;
extern bool currentSensorInitialized;

void initTMAG5273Callback();
float readTMAG5273Callback();

extern GenericSensor sensorFOC;

extern BLDCMotor motor;
extern BLDCDriver6PWM driver;
extern float target;
extern bool direction; // Motor direction. FALSE - counter-clockwise; TRUE - clockwise
extern TaskHandle_t focTask;

void controlMotorTask( void * parameter);

void focBLDCSetup();

#endif // FOC_BLDC_H