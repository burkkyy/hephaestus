#include "window.hpp"

#include "util/logger.hpp"

namespace hep {

Window::Window(u32 width, u32 height, const std::string& name)
    : width{width}, height{height}, name{name} {
  initialize();
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Window::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    log::fatal("Failed to create VkSurfaceKHR.");
    throw std::exception();
  }
  log::info("Created VkSurfaceKHR.");
}

void Window::resizeCallback(GLFWwindow* window, u32 width, u32 height) {
  (void)window;
  (void)width;
  (void)height;
}

void Window::initialize() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
}

}  // namespace hep
