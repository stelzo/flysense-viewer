cmake_minimum_required(VERSION 3.10)
project(flysense-viewer VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(PROJECT_SOURCE_DIR src)

find_package(OpenCV REQUIRED)

#include_directories( ${OpenCV_INCLUDE_DIRS}  include )
#add_compile_options(-Ofast)
#include_directories(include)

file(GLOB SOURCES
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.cc
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include /home/nvidia/flysense-camera-interaction/include)

add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SOURCE_DIR}/viewer.cc
)

include(GNUInstallDirs)

add_custom_target(uninstall
    xargs rm < install_manifest.txt
)

add_executable(${PROJECT_NAME}
    SOURCES
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS} /home/nvidia/flysense-camera-interaction/build/libflysense-jetson-cam.so)
