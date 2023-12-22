#ifndef FOC_BLDC_H
#define FOC_BLDC_H

#include <SimpleFOC.h>
#include <SparkFun_TMAG5273_Arduino_Library.h>
#include <Wire.h>
#include <mutex>

extern std::mutex sensorFOCMutex;
extern TMAG5273 sensor;
extern bool angleSensorInitialized;
extern bool currentSensorInitialized;

void initTMAG5273Callback();
float readTMAG5273Callback();

extern GenericSensor sensorFOC;

extern BLDCMotor motor;
extern BLDCDriver6PWM driver;
extern float target;
extern TaskHandle_t focTask;

void controlMotorTask( void * parameter);

void focBLDCSetup();

#endif // FOC_BLDC_H