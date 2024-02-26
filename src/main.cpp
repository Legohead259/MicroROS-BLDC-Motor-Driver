#include "micro_ros_bldc.h"
#include "foc_bldc.h"
#include "neopixel.h"

void setup() {
    Wire.begin();
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);

    Serial1.print("Starting LED state machine...");
    ledStateMachine.begin();
    Serial1.println("Done!");

    microROSNodeSetup();
    focBLDCSetup();
}

void loop() {

}
