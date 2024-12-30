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

void Window::createSurface(const vk::Instance& instance,
                           vk::SurfaceKHR& surface) {
  VkSurfaceKHR rawSurface;
  if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window,
                              nullptr, &rawSurface) != VK_SUCCESS) {
    log::fatal("Failed to create vk::SurfaceKHR.");
    throw std::exception();
  }

  surface = vk::SurfaceKHR(rawSurface);
  log::verbose("Created vk::SurfaceKHR.");
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
