#include "micro_ros_bldc.h"
#include "foc_bldc.h"

#define RX1_PIN 15
#define TX1_PIN 12

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);
    Wire.begin();

    focBLDCSetup();
    microROSNodeSetup();
}

void loop() {
    // Execute pending tasks in the executor. This will handle all ROS communications.
    // RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
    char _statsBuf[200];
    vTaskGetRunTimeStats(_statsBuf);
    Serial1.printf(_statsBuf);
    delay(100);
}
