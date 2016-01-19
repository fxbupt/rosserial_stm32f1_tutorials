cmake_minimum_required(VERSION 2.8.3)
project(rosserial_stm32f1_tutorials)

find_package(catkin REQUIRED COMPONENTS
  rosserial_client
  rosserial_stm32f1
  std_msgs
)

catkin_package(
  CATKIN_DEPENDS std_msgs
)

rosserial_generate_ros_lib(
  PACKAGE rosserial_stm32f1
  SCRIPT make_libraries
)

# Chatter tutorial
rosserial_configure_client(
  DIRECTORY chatter
  TOOLCHAIN_FILE ${ROSSERIAL_STM32_TOOLCHAIN}
)
rosserial_add_client_target(chatter chatter ALL)
rosserial_add_client_target(chatter chatter.bin)
rosserial_add_client_target(
  chatter 
  flash 
  st-flash write ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_SHARE_DESTINATION}/chatter.bin 0x08000000
)
rosserial_add_client_target(
  chatter 
  size 
  arm-none-eabi-size -A -d ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_SHARE_DESTINATION}/chatter
)
