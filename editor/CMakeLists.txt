set(NAME alphane-editor)
set(ENGINE_NAME ${PROJECT_NAME})

file(GLOB_RECURSE SOURCES *.cpp src/*.cpp)

add_library(editor-deps INTERFACE)

target_link_libraries(editor-deps INTERFACE
  ${ENGINE_NAME}
  vulkan
  shaderc
  glfw
  glm
  imgui
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

add_executable(${NAME} main.cpp)

target_link_libraries(${NAME} PRIVATE editor-deps)

target_include_directories(${NAME} PRIVATE 
    "${CMAKE_SOURCE_DIR}/editor/src"
    "${CMAKE_SOURCE_DIR}/engine/include" 
    "${CMAKE_SOURCE_DIR}/external/imgui"
)
