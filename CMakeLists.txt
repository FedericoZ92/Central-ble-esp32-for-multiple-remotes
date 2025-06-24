# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly

# Message for debugging purposes
message(STATUS "project cmakelists")

# Set the target platform for the project
set(IDF_TARGET esp32s3)

# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.16)

# Include ESP-IDF's common project settings
message(STATUS "project cmakelists: include project.cmake")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

message(STATUS "src cmakelists")

# Set the project name
project(bleButton)

# Enable colored diagnostics in the compiler
add_compile_options(-fdiagnostics-color=always)



