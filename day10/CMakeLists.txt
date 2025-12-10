cmake_minimum_required(VERSION 3.16)

# Compiler settings
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(day day10)

include(FetchContent)

FetchContent_Declare(
    highs
    GIT_REPOSITORY https://github.com/ERGO-Code/HiGHS.git
)

FetchContent_MakeAvailable(highs)

add_executable(${day} ${day}.cpp)

# Include directory (optional)
target_include_directories(${day} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${day} PRIVATE ctre::ctre highs)

get_target_property(HIGHS_INCLUDE_DIRS highs INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(day10 SYSTEM PRIVATE ${HIGHS_INCLUDE_DIRS})


# Useful warnings
target_compile_options(${day} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)