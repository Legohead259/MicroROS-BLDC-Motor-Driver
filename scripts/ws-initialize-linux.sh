#! /bin/bash

# Update the internal packages and upgrade to latest versions
sudo apt update && apt upgrade -y

# Install and configure git
sudo apt install git -y
git config --global user.name Legohead259
git config --global user.email legohead259@gmail.com

# Install ROS2 (https://docs.ros.org/en/iron/Installation/Ubuntu-Install-Debians.html)
sudo apt install software-properties-common -y
sudo add-apt-repository universe -y
sudo apt update && sudo apt install curl -y
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null
sudo apt update && sudo apt install ros-dev-tools -y
sudo apt install ros-iron-desktop -y

# Setup ROS2 to be CLI accessible
source /opt/ros/iron/setup.bash

# Ensure all future terminals can access the `ros2` CLI
echo "source /opt/ros/iron/setup.bash" >> ~/.bashrc

# Get Python-Venv (necessary for PlatformIO: https://community.platformio.org/t/platformio-ide-cant-find-python-interpreter-on-linux/24262/2)
sudo apt update && apt-get install python3-venv

# Get the other repositories
git clone https://github.com/Legohead259/ROS2-Micro-ROS-ESC-Workspace ~/ROS2-Micro-ROS-ESC-Workspace
git clone https://github.com/Legohead259/MicroROS-ESC-Interfaces ~/MicroROS-ESC-Interfaces
cd ~/ROS2-Micro-ROS-ESC-Workspace && git submodule update --init
cd ~/MicroROS-ESC-Interfaces && git submodule update --init

# Install Micro-ROS
cd ROS2-Micro-ROS-ESC-Workspace/
rosdep init
sudo apt update && rosdep update
rosdep install --from-paths src --ignore-src -y
colcon build
source install/local-setup.bash

ros2 run micro_ros_setup create_agent_ws.sh
ros2 run micro_ros_setup build_agent.sh
source install/local_setup.bash

# Restart system
sudo reboot
