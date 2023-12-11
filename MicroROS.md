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