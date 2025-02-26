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

add_subdirectory(external/glfw)
add_subdirectory(external/glm)

# Store source files
file(GLOB_RECURSE SRC_FILES src/*.cpp)

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
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR} 
  OUTPUT_NAME app.bin
)
