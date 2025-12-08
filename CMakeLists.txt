cmake_minimum_required(VERSION 3.10.0)
project(AOC2025 VERSION 0.1.0 LANGUAGES C CXX)


# Compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include(FetchContent)
FetchContent_Declare(
    ctre
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
    GIT_TAG origin/main
)
FetchContent_MakeAvailable(ctre)


# Copy the inputs directory to the build folder
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/inputs
            ${CMAKE_BINARY_DIR}/inputs
)

message(${CMAKE_SOURCE_DIR})
message(${CMAKE_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR}/utils)

add_executable(AOC2025 main.cpp)


#add_subdirectory(day1)
add_subdirectory(day2)
add_subdirectory(day3)
add_subdirectory(day4)
add_subdirectory(day5)
add_subdirectory(day6)
add_subdirectory(day7)
add_subdirectory(day8)