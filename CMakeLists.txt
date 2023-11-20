cmake_minimum_required(VERSION 3.5)

project(RoboticICSI-Laser-DATABASE VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# CUDA
set(CUDA_TOOLKIT_ROOT_DIR "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.3")
find_package(CUDA 11 REQUIRED)

set(CMAKE_CUDA_STANDARD 11)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# !CUDA

# OpenCV
set(OpenCV_DIR "D:/opencv470/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# !OpenCV

set(PROJECT_SOURCES
    MainFrm.h
    MainFrm.cpp
    inference.h
    inference.cpp
)

add_executable(RoboticICSI-Laser-DATABASE ${PROJECT_SOURCES})
target_link_libraries(RoboticICSI-Laser-DATABASE ${OpenCV_LIBS})
