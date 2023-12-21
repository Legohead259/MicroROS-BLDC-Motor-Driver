#include "micro_ros_bldc.h"
#include "foc_bldc.h"
#include "neopixel.h"

void setup() {
    Wire.begin();
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);

    sensor.begin(TMAG5273_I2C_ADDRESS_INITIAL);

    ledStateMachine.begin();
    microROSNodeSetup();
    focBLDCSetup();
}

void loop() {

}
