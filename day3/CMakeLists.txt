cmake_minimum_required(VERSION 3.16)
project(day1 VERSION 1.0 LANGUAGES CXX)

# Compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(day3 day3.cpp)

# Include directory (optional)
target_include_directories(day3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(day3 PRIVATE ctre::ctre)



# Useful warnings
target_compile_options(day3 PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)