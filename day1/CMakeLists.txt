cmake_minimum_required(VERSION 3.16)
project(day1 VERSION 1.0 LANGUAGES CXX)

# Compiler settings
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable (expects main.cpp in the same directory)
add_executable(day1 day1.cpp)

# Include directory (optional)
target_include_directories(day1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(day1 PRIVATE ctre::ctre)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(day1 PRIVATE -fdirectives-only
)
endif()

# Useful warnings
target_compile_options(day1 PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)