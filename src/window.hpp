#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vulkan/vulkan.hpp>

#include "events/event.hpp"
#include "util/types.hpp"

namespace hep {

class Window {
 public:
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  Window(int width, int height, const std::string& name = "hephaestus");
  ~Window();

  bool shouldClose() { return glfwWindowShouldClose(window); }

  vk::Extent2D getExtent() {
    return {static_cast<u32>(width), static_cast<u32>(height)};
  }
  void createSurface(const vk::Instance& instance, vk::SurfaceKHR& surface);
  bool wasResized() const { return this->resized; };
  void resetResizedFlag() { this->resized = false; }

 private:
  static void resizeCallback(GLFWwindow* window, int width, int height);
  static void keyEventCallback(GLFWwindow* window, int key, int scancode,
                               int action, int mods);
  void initialize();

  int width, height;
  bool resized = false;
  std::string name;
  GLFWwindow* window;
};

}  // namespace hep
