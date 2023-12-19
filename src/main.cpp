#include "micro_ros_bldc.h"
#include "foc_bldc.h"
#include "neopixel.h"

#define RX1_PIN 15
#define TX1_PIN 12

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
    Wire.begin();

    sensor.begin(TMAG5273_I2C_ADDRESS_INITIAL);

    ledStateMachine.begin();
    // focBLDCSetup();
    microROSNodeSetup();
}

void loop() {
    // Serial1.printf("LED tasking running on Core %d\r\n", xPortGetCoreID());
    // Serial1.printf("Current state: %d\r\n", systemState);
    // for(;;) {
    //     ledStateMachine.executeState();
    // }
    // ledStateMachine.executeState();
}
