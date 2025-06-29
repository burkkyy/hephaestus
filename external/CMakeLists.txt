# 1. Vulkan - shaderc, spirv-reflect
add_library(vulkan INTERFACE)

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

# Adding SPIRV-Reflect to vulkan interface
target_sources(vulkan INTERFACE 
  ${Vulkan_INCLUDE_DIRS}/SPIRV-Reflect/spirv_reflect.c
)

target_include_directories(vulkan INTERFACE 
  ${Vulkan_INCLUDE_DIRS}/SPIRV-Reflect
)

target_link_libraries(vulkan INTERFACE
  Vulkan::Vulkan
  Vulkan::shaderc_combined
)

# 3. GLFW (static)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
set(GLFW_VULKAN_STATIC ON CACHE BOOL "Link Vulkan statically with GLFW" FORCE)
add_subdirectory(glfw)

# 4. GLM (static)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glm)

# 5. IMGUI (static)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_demo.cpp   # remove this 
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_DIR}/backends
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(imgui PRIVATE
  glfw
  vulkan
)

if(UNIX AND NOT APPLE)
  target_compile_options(imgui PRIVATE -fPIC)
endif()
