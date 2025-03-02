cmake_minimum_required(VERSION 3.30.0)

project(hephaestus VERSION 0.0.0)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to release build
# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE release)
# endif()

# if(CMAKE_BUILD_TYPE STREQUAL "release")
#     add_definitions(-DNDEBUG)
# endif()

# Download git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  option(GIT_SUBMODULE "Check submodules during build" ON)
  if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, checkout submodules")
    endif()
  endif()
endif()

find_package(Vulkan REQUIRED)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)

# Store source files
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# Store shader files and define build rules
find_program(GLSL_VALIDATOR glslangValidator HINTS 
${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
/usr/bin 
/usr/local/bin 
${VULKAN_SDK_PATH}/Bin
${VULKAN_SDK_PATH}/Bin32
)
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

# Define Executable
add_executable(${PROJECT_NAME} app.cpp ${SRC_FILES} ${SPIRV_SHADERS})

target_include_directories(${PROJECT_NAME}
  PUBLIC external/glfw/include
  PUBLIC external/glm/include
)
  
target_link_libraries(${PROJECT_NAME}
  PRIVATE glfw
  PRIVATE glm
  PRIVATE Vulkan::Vulkan
  PRIVATE dl
  PRIVATE pthread
  PRIVATE X11
  PRIVATE Xxf86vm
  PRIVATE Xrandr
  PRIVATE Xi
)
  
set_target_properties(${PROJECT_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} 
  OUTPUT_NAME app.bin
)

add_dependencies(${PROJECT_NAME} ${SHADER_DIR})
