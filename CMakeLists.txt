cmake_minimum_required(VERSION 3.7)
project(ImageTransmission)
find_package(OpenCV)
set(CMAKE_CXX_STANDARD 11)
include_directories(${OpenCV_INCLUDE_DIRS})
set(SOURCE_FILES main.cpp)
add_executable(ImageTransmission ${SOURCE_FILES})

target_link_libraries(ImageTransmission ${OpenCV_LIBS})
