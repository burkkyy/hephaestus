cmake_minimum_required(VERSION 3.22.2)

set(NAME testbed)
project(${NAME})

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEPHAESTUS_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/../include)
set(HEPHAESTUS_LIBRARIES ${CMAKE_BINARY_DIR}/../build/libhephaestus.so)

add_executable(testbed main.cpp)

target_include_directories(testbed PRIVATE ${HEPHAESTUS_INCLUDE_DIRS})

find_library(HEPHAESTUS_LIB NAMES hephaestus HINTS "${CMAKE_SOURCE_DIR}/../build")
if(NOT HEPHAESTUS_LIB)
  message(FATAL_ERROR "libhephaestus.so not found in ../build/lib")
else()
  message(STATUS "Found libhephaestus.so")
endif()

target_link_libraries(testbed PRIVATE ${HEPHAESTUS_LIBRARIES})

set_target_properties(testbed PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  OUTPUT_NAME testbed.bin
  BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
)
