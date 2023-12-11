# Usage
## Custom ROS2 Packages
MicroROS will automatically build ROS2 packages using `colcon`, similar to the full build.
The PlatformIO library includes all the toolchains by default and requires that custom packages are put into a directory called `extra_packages/` within the root directory.
For example: `BLDC-Motor-Driver/extra_packages/motor_interfaces` is a custom ROS2 interface package that is built into MicroROS.

To build the custom packages, the entire MicroROS library needs to be rebuilt.
The built source files will be located in a directory called: `{project_name}/.pio/{env_name}/micro_ros_platformio/libmicroros/include/{custom_package_name}`.
They can be included in an application script with `#include <{custom_package_name}/{action,msg,srv}/{interface_name}.h>`
If the library is already built (`libros/` directory present) then it must be manually deleted.
Alternatively, you can execute the following command in a terminal located in the root project directory:

```
pio run --target clean_microros
```

This command is also listed in the "Custom" project tasks tab in the PlatformIO VSCode extension page.
When the `libmicroros/` directory is gone, build the project.
This will compile the custom package automatically.

| :exclamation:  Due to long compilation times (>4 minutes), it would be prudent to test build the package in the ROS2 environment first to ensure everything is good.  |
|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------|


### Custom Actions
[TODO]

### Custom Messages
[TODO]

### Custom Services
[TODO]

### MicroROS Meta Parameters
To conserve memory, MicroROS artificially limits number of nodes, services, publishers, etc. that can be deployed by a single application.
These parameters are defined in `micro_ros_platformio/metas/colcon.meta`.
The parameters and their defaults are explained in the table below:

| Parameter                     | Usage                                                         | Default
|-------------------------------|---------------------------------------------------------------|----------
| RMW_UXRCE_MAX_NODES           | Limits the number of nodes                                    | 1
| RMW_UXRCE_MAX_PUBLISHERS      | Limits the number of topic publishers                         | 10
| RMW_UXRCE_MAX_SUBSCRIPTIONS   | Limits the number of topic subscriptions                      | 5
| RMW_UXRCE_MAX_SERVICES        | Limits the number of available services                       | 1
| RMW_UXRCE_MAX_CLIENTS         | Limits the number of clients                                  | 1
| RMW_UXRCE_MAX_HISTORY         | [TODO]                                                        | 4
| RMW_UXRCE_TRANSPORT           | Lists the DDS transport technique (custom, serial, UDP, etc.) | custom
| RMW_UXRCE_MAX_GUARD_CONDITION | Handles the number of guard conditions (timers) allowed       | 4

By changing these parameters, the MicroROS script can be expanded to handle complex server/client or pub/sub applications.