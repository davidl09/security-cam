cmake_minimum_required(VERSION 3.27)
project(cam)

set(CMAKE_CXX_STANDARD 23)
set(OpenCV_DIR "${/usr/lib/x86_64-linux-gnu/cmake/opencv4/OpenCVConfig.cmake}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

find_package(OpenCV REQUIRED)

add_executable(cam main.cpp)

target_include_directories(cam PUBLIC ${OpenCV_INCLUDE_DIRS})

target_link_libraries(cam PUBLIC ${OpenCV_LIBS})
