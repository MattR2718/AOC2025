cmake_minimum_required(VERSION 3.16)

# Compiler settings
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(day day7)

add_executable(${day} ${day}.cpp)

# Include directory (optional)
target_include_directories(${day} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${day} PRIVATE ctre::ctre)



# Useful warnings
target_compile_options(${day} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)