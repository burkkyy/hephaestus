set(NAME testbed)
set(ENGINE_NAME ${PROJECT_NAME})

file(GLOB_RECURSE SOURCES *.cpp src/*.cpp)

add_library(deps INTERFACE)

target_link_libraries(deps INTERFACE
  ${ENGINE_NAME}
  vulkan
  shaderc
  glfw
  glm
  imgui
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

add_executable(testbed main.cpp)

target_link_libraries(testbed PRIVATE deps)

target_include_directories(testbed PRIVATE 
    "${CMAKE_SOURCE_DIR}/engine/include" 
    "${CMAKE_SOURCE_DIR}/external/imgui"
)
