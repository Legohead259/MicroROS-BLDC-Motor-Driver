# File: motor_interfaces/msg/AngularPosition.msg
#
# motor_interfaces: ROS 2 package containing custom messages, services and actions for the control of a BLDC motor using SimpleFOC and MicroROS
# AngularPosition.msg: Message definition for reporting the measured angular position as reported by the position sensor
#
# Description:
# The message reports the angular position recorded by the on-board position sensor in rad.
# Note that the position is relative to the polarity of a diametric magnet on the shaft and not the mounting position of the motor.
# Specific characterization and calibration of a "0" position for the motor mount should be done externally
#
# In an error state, the `angular_position` and `error` fields will report -999
#
# Fields:
# uint32    timestamp           Microcontroller timestamp in milliseconds (ms) since start
# float32   angular_position    The angular position of the motor shaft in rad
# float32   error               The estimated error with the measurement in rad

uint32 timestamp
float32 angular_position
float32 error