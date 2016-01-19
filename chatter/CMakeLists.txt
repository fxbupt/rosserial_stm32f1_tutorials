cmake_minimum_required(VERSION 2.8.3)
project(chatter)

include_directories(${ROS_LIB_DIR})

# rosserial specific definitions
add_definitions(-DROSSERIAL_BAUDRATE=57600)
add_definitions(-DTX_BUFFER_SIZE=256)
add_definitions(-DRX_BUFFER_SIZE=256)

# Project Hardware specific definition
set(STM32_CHIP STM32F103C8)
set(CMAKE_BUILD_TYPE Release)

# Set cmake modules path for project
get_filename_component(CMAKE_STM32_MODULE_PATH ${CMAKE_TOOLCHAIN_FILE} DIRECTORY)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_STM32_MODULE_PATH}/cmake/Modules)

# Required packages for building STM32 project
enable_language(ASM)
find_package(CMSIS REQUIRED)
find_package(StdPeriphLib REQUIRED)

set(STM32_LINKER_SCRIPT ${CMSIS_LINKER_SCRIPT})
# For some reason there's -rdynamic flag
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-specs=nosys.specs")
# Compile flags for disabling C++ exceptions and RTTI
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
# Doesn't hide _init required by __libc_init_array
string(REPLACE "-nostartfiles" ""  CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMSIS_INCLUDE_DIR}
  ${StdPeriphLib_INCLUDE_DIR}
)

# Project source files
set(PROJECT_SOURCES
  chatter.cpp
  startup_stm32f10x_md.s
)

# Add sources to be compiled
# Custom startup, so we call __libc_init_array before entering main.
# Usual ${CMSIS_STARTUP_SOURCE}
add_executable(${CMAKE_PROJECT_NAME} ${ROS_LIB_DIR}/STM32Hardware.cpp ${ROS_LIB_DIR}/time.cpp ${ROS_LIB_DIR}/ringbuffer.c ${PROJECT_SOURCES} ${CMSIS_SOURCES} ${StdPeriphLib_SOURCES} )
# Link required libraries
target_link_libraries(${CMAKE_PROJECT_NAME} gcc c m nosys)

stm32_set_target_properties(${CMAKE_PROJECT_NAME})
stm32_add_hex_bin_targets(${CMAKE_PROJECT_NAME})
