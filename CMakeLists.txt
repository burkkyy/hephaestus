cmake_minimum_required(VERSION 3.22.2)

set(NAME hephaestus)

project(${NAME} VERSION 0.0.0)

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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
elseif(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")  # Static runtime for MSVC
endif()

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

# 1. Set VULKAN_SDK_PATH in .env.cmake to target specific vulkan version
if (DEFINED VULKAN_SDK_PATH)
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
  set(Vulkan_FOUND "True")
else()
  find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
else()
	message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# 2. GLFW (static link)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_VULKAN_STATIC ON CACHE BOOL "Link Vulkan statically with GLFW" FORCE)
add_subdirectory(external/glfw)
set(GLFW_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/glfw/include")
set(GLFW_LIBRARIES glfw)

# 3. GLM (static link)
add_subdirectory(external/glm)
set(GLM_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/external/glm/include")
set(GLM_LIBRARIES glm)

# 4. Store source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 5. Store shader files and define build rules
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

# Define Executable

# Platform specific
if(UNIX AND NOT APPLE)  # Linux
  message(STATUS "Creating build for linux")

  add_library(${PROJECT_NAME} SHARED ${SOURCES})

  target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
  )

  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GLFW_LIBRARIES}
    ${GLM_LIBRARIES}
    ${Vulkan_LIBRARIES}
    dl
  )

  set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME hephaestus
  )

  target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
elseif(WIN32)  # Windows
  message(STATUS "Creating build for windows")

  add_executable(${PROJECT_NAME} app.cpp ${SOURCES} ${SPIRV_SHADERS})

  target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
  )

  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GLFW_LIBRARIES}
    ${GLM_LIBRARIES}
    ${Vulkan_LIBRARIES}
  )

  set_target_properties(${PROJECT_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} 
    OUTPUT_NAME app
  )
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
add_dependencies(${PROJECT_NAME} ${SHADER_DIR})
