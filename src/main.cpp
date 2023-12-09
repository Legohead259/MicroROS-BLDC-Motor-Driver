#include "micro_ros_bldc.h"
#include "foc_bldc.h"

void setup() {
    focBLDCSetup();

    Serial.begin(115200);
    Serial.println("Motor ready!");
    Serial.println("Set target velocity [rad/s]");
    _delay(1000);
    microROSNodeSetup();
}

void loop() {
    // Execute pending tasks in the executor. This will handle all ROS communications.
    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}
