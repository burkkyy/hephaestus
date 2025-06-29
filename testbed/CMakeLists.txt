cmake_minimum_required(VERSION 3.22.2)

set(NAME testbed)
project(${NAME})

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(HEPHAESTUS_LIB 
    NAMES hephaestus 
    HINTS "${CMAKE_SOURCE_DIR}/../build"
    REQUIRED
)

add_executable(testbed main.cpp)

target_include_directories(testbed PRIVATE "${CMAKE_SOURCE_DIR}/../include")
target_link_libraries(testbed PRIVATE ${HEPHAESTUS_LIB})

set_target_properties(testbed PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME testbed.bin
    BUILD_RPATH "${CMAKE_BINARY_DIR}/../build"
)
