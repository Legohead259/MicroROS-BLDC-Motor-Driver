# File: motor_interfaces/msg/AngularVelocity.msg
# 
# motor_interfaces: ROS 2 package containing custom messages, services and actions for the control of a BLDC motor using SimpleFOC and MicroROS
# AngularVelocity.msg: Message definition for reporting the measured angular velocity as calculated by the time derivative of the position sensor
#
# Description:
# The message reports the angular velocity recorded by the time derivative of the on-board position sensor in rad/sec.
# Note that the velocity is subject to errors and drifts
# Specific characterization and calibration has not been performed.
# Use with caution.
#
# In an error state, the `angular_velocity` and `error` fields will report -999
#
# Fields:
# uint32    timestamp           Microcontroller timestamp in milliseconds (ms) since start
# float32   angular_position    The angular velocity of the motor shaft as determined by time derivative of position in rad/sec
# float32   error               The estimated error with the measurement

uint32 timestamp
float32 angular_velocity
float32 error