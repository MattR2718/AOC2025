cmake_minimum_required(VERSION 3.10.0)
project(AOC2025 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(AOC2025 main.cpp)

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

#add_subdirectory(day1)
add_subdirectory(day2)