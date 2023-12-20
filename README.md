# Micro-ROS Electronic Speed Controller Firmware
This repository holds the firmware for a Micro-ROS based electronic speed controller (ESC).
The primary goal of this firmware is to provide a base for building and deploying a Micro-ROS application and using it with hardware.
To make the project more difficult, the board running Micro-ROS will also use a Field-Oriented Control algorithm developed by the SimpleFOC team to demonstrate reliable control of a brushless DC (BLDC) motor.

## Objective
The objective of this project is to design, build, test, and possibly sell an ESC that natively interacts with the Robot Operating System 2 (ROS2) used widely in hobbyist- and prosumer-grade robotics platforms.
This feature will seamlessly bridge the gap between the host PC running ROS2 and the fine control of a BLDC motor required by some projects.

## "Big-R" Requirements
The "Big-R" or overarching requirements fundamentally drive the project - they are the reason for its existence:
* **R1)** The system shall use Micro-ROS to interact with a host PC
* **R2)** The system shall be able to control a BLDC motor using Field Oriented Control algorithms
* **R3)** The system shall support multiple transport methods to the ROS2 network
* **R4)** The system shall be entirely open source and accessible

## Derived Capabilities
On the software side, the derived capabilities are those that are required for the software to function.
They are separated into three tiers:
 * **Threshold**: Those capabilities which are required for a minimally viable product (MVP)
 * **Reach**: Capabilities that add some value to the product while not requiring extraordinary effort
 * **Stretch**: The capabilities which are radical and add significant or unique value to the project, but at a high development cost

### Threshold Capabilities
- [ ] **1000 -** The system shall be programmable via USB
- [ ] **1001 -** The system shall be able to run a ROS2 node with multiple entities
- [ ] **1002 -** The system shall be able to run a ROS2 and FOC algorithm simultaneously
- [ ] **1003 -** The system shall be able to control the BLDC motor in an open-loop velocity mode
- [ ] **1004 -** The system shall be able to set a target velocity during runtime
- [ ] **1005 -** The system shall publish basic diagnostic data about the kode
- [ ] **1006 -** The system shall be able to change the motor direction
- [ ] **1007 -** The system shall communicate with the host PC via a serial transport layer
- [ ] **1008 -** All system code shall be documented thoroughly
- [ ] **1009 -** All system code shall be backed up and controlled using VCS

### Reach Capabilities
- [ ] **2000 -** The system shall be able to control the BLDC motor with multiple control modes
- [ ] **2001 -** The system shall be able to change the control modes during runtime
- [ ] **2002 -** The system shall be able to report advanced diagnostic data about the motor
- [ ] **2003 -** The system shall be able to report advanced diagnostic data about the FOC algorithm
- [ ] **2004 -** The system shall publish data from its FOC sensors
- [ ] **2005 -** The system shall have a debugging console
- [ ] **2006 -** The system shall use an RGB LED for basic operator feedback about the system state
- [ ] **2007 -** The system shall have its basic FOC parameters configurable during runtime

### Stretch Capabilities
- [ ] **3000 -** The system will have a "homing" feature for the closed-loop position control mode
- [ ] **3001 -** The system can configure a "home" position during runtime
- [ ] **3002 -** The system can support a CANbus transport layer
- [ ] **3003 -** The system can support a hardwired Internet protocol (UDP) transport layer
- [ ] **3004 -** The system can support a wireless Internet protocol (UDP) transport layer
- [ ] **3005 -** The system can have its ROS parameters changed during launch
