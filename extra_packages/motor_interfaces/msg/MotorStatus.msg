# File: motor_interfaces/msg/MotorStatus.msg
#
# motor_interfaces: ROS 2 package containing custom messages, services and actions for the control of a BLDC motor using SimpleFOC and MicroROS
# MotorStatus.msg: Custom message definition for motor health and operation information
#
# Description:
# The message encapsulates several fields regarding the motor status using the SimpleFOC library
#
# Fields:
# uint32    timestamp   Micrcontroller timestamp in milliseconds (ms) since start
# bool      enabled     If the motor is able to run
# bool      direction   The direction the motor will spin; FALSE - counter-clockwise, TRUE - clockwise
# uint8     status      The motor status as reported by the FOC. Refer to `FOCMotorStatus` in `FOCMotor.h` of the SimpleFOC library

uint32 timestamp
bool enabled
bool direction 
uint8 mode
uint8 status