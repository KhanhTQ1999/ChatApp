cmake_minimum_required(VERSION 3.10)
project(ChatApp)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type: Debug or Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
)

# Collect all source files
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/common/*.cpp
    ${PROJECT_SOURCE_DIR}/model/*.cpp
    ${PROJECT_SOURCE_DIR}/views/*.cpp
    ${PROJECT_SOURCE_DIR}/controller/*.cpp
    ${PROJECT_SOURCE_DIR}/services/*.cpp
    ${PROJECT_SOURCE_DIR}/ui/*.cpp
    ${PROJECT_SOURCE_DIR}/utils/*.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

# Create executable
add_executable(ChatApp ${SOURCES})

# Optional: define macro for build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ChatApp PRIVATE BUILD_DEBUG)
else()
    target_compile_definitions(ChatApp PRIVATE BUILD_RELEASE)
endif()

# Optional: enable colored logging
target_compile_definitions(ChatApp PRIVATE LOG_USE_COLOR)

# Link pthread for threading support
find_package(Threads REQUIRED)
target_link_libraries(ChatApp PRIVATE Threads::Threads)
