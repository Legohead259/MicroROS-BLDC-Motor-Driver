#include "foc__angle_sensor.h"

TMAG5273 sensor;

void initTMAG5273Callback() {
    Serial1.print("Starting TMAG5273...");
    if(!sensor.begin(TMAG5273_I2C_ADDRESS_INITIAL)) { 
        Serial1.println("Failed to start TMAG5273");
        while(1); // Stop further code execution
    }
    sensor.setConvAvg(TMAG5273_X32_CONVERSION);
    sensor.setAngleEn(0x01);
    Serial1.println("Done!");
}

float readTMAG5273Callback() {
    return sensor.getAngleResult() / 180 * PI;
}