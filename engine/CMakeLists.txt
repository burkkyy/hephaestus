set(ENGINE_NAME ${PROJECT_NAME})

add_library(${ENGINE_NAME} STATIC)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

target_sources(${ENGINE_NAME} PRIVATE 
  ${SOURCES} 
  ${HEADERS}
)

target_include_directories(${ENGINE_NAME} PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/external/glfw/include"
  "${CMAKE_SOURCE_DIR}/external/glm"
  "${CMAKE_SOURCE_DIR}/external/imgui"
  "${CMAKE_SOURCE_DIR}/external/imgui/backends"
  "${CMAKE_SOURCE_DIR}/external/entt/single_include"
)
