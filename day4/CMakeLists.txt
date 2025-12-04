cmake_minimum_required(VERSION 3.16)
project(day4 VERSION 1.0 LANGUAGES CXX)

# Compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(day4 day4.cpp)

# Include directory (optional)
target_include_directories(day4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(day4 PRIVATE ctre::ctre)



# Useful warnings
target_compile_options(day4 PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)