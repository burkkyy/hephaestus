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

# Below is all the shader stuff
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  ${VULKAN_SDK}/Bin
  ${VULKAN_SDK}/Bin32
)
if(NOT GLSL_VALIDATOR)
  message(FATAL_ERROR "glslangValidator not found. Install Vulkan SDK or provide it in PATH.")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

set(SHADER_DIR shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${SHADER_DIR})

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
  ${SHADER_DIR}
  DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${PROJECT_NAME} ${SHADER_DIR})
