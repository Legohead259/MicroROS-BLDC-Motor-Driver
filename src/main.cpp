#include "micro_ros_bldc.h"
#include "foc_bldc.h"

void setup() {
    Serial.begin(115200);
    // focBLDCSetup();
    microROSNodeSetup();
}

void loop() {
    // Execute pending tasks in the executor. This will handle all ROS communications.
    // RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
    // motor.loopFOC();
    // motor.move(target);
}
