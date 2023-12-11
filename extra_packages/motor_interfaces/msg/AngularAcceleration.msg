# File: motor_interfaces/msg/AngularAcceleration.msg
#
# motor_interfaces: ROS 2 package containing custom messages, services and actions for the control of a BLDC motor using SimpleFOC and MicroROS
# AngularAcceleration.msg: Message definition for reporting the measured angular acceleration as reported by the position sensor
#
# Description:
# !!EXPERIMENTAL!!
# The message reports the angular acceleration recorded by the on-board position sensor in rad/sec/sec.
# Note that this report is subject to drift and high errors
# Compensation or characterization have not been done at this time.
# Use with caution.
#
# In an error state, the `angular_acceleration` and `error` fields will report -999
#
# Fields:
# uint32    timestamp               Microcontroller timestamp in milliseconds (ms) since start
# float32   angular_acceleration    The angular acceleration of the motor shaft as a time derivative of the angular position in rad/sec/sec.
# float32   error                   The estimated error with the measurement in rad/sec/sec

uint32 timestamp
float32 angular_acceleration
float32 error