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


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SOURCE_DIR}/viewer.cc
    #${PROJECT_SOURCE_DIR}/cameraManage.h
)

include(GNUInstallDirs)

add_custom_target(uninstall
    xargs rm < install_manifest.txt
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER ${PROJECT_SOURCE_DIR}/viewer.h
)

install(TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/flysense-viewer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS} flysense-jetson-cam httpserver)
