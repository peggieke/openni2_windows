### CMakeLists for Ni2PointCloud ###

cmake_minimum_required (VERSION 3.1)

set(TARGET_SAMPLE_NAME Ni2PointCloud)

project(${TARGET_SAMPLE_NAME})

## Setup Building Environment
include(${PROJECT_SOURCE_DIR}/../CMake/SetupBuildEnvironment.cmake)
include(${PROJECT_SOURCE_DIR}/../CMake/SetupGLFW.cmake)

## Build Ni2PointCloud Executable
add_executable(${TARGET_SAMPLE_NAME}
    ${PROJECT_SOURCE_DIR}/Source/Ni2PointCloud.cpp
    ${PROJECT_SOURCE_DIR}/Source/Ni2PointCloud.h
)

add_dependencies(${TARGET_SAMPLE_NAME} glfw)

target_include_directories(${TARGET_SAMPLE_NAME} PRIVATE ${OPENNI2_INC})
target_include_directories(${TARGET_SAMPLE_NAME} PRIVATE ${GLFW_INC})

target_link_libraries(${TARGET_SAMPLE_NAME} ${OPENNI2_LIB})
target_link_libraries(${TARGET_SAMPLE_NAME} ${GLFW_LIB})

## Installation
install(TARGETS ${TARGET_SAMPLE_NAME} DESTINATION .)