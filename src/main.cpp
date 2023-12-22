#include "micro_ros_bldc.h"
#include "foc_bldc.h"
#include "neopixel.h"

void IRAM_ATTR positionISR() {
    shaftAngle = sensor.getAngleResult() / 180 * PI;
}

void setup() {
    Wire.begin();
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, RX1_PIN, TX1_PIN);

    Serial1.print("Starting TMAG5273...");
    if(!sensor.begin(TMAG5273_I2C_ADDRESS_INITIAL)) { 
        Serial1.println("Failed to start TMAG5273");
        while(1); // Stop further code execution
    }
    sensor.setConvAvg(TMAG5273_X32_CONVERSION);
    sensor.setAngleEn(0x01);
    sensor.setInterruptResult(TMAG5273_INTERRUPT_ASSERTED);
    sensor.setThresholdEn(false);
    // sensor.setIntPinState(true);
    sensor.setInterruptMode(TMAG5273_INTERRUPT_THROUGH_INT);
    sensor.setMaskInterrupt(TMAG5273_INTERRUPT_ENABLED);
    angleSensorInitialized = true;
    Serial1.println("Done!");

    Serial1.print("Attaching ISR...");
    pinMode(TMAG5273_INT, INPUT_PULLUP);
    // attachInterrupt(TMAG5273_INT, positionISR, FALLING);
    Serial1.println("Done!");

    Serial1.print("Starting LED state machine...");
    ledStateMachine.begin();
    Serial1.println("Done!");

    microROSNodeSetup();
    focBLDCSetup();
}

void loop() {

}
