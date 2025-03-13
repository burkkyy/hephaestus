#include "window.hpp"

#include "events/key_event.hpp"
#include "util/logger.hpp"

namespace hep {

Window::Window(int width, int height, const std::string& name)
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
  log::trace("Created vk::SurfaceKHR.");
}

void Window::keyEventCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
  if (action == GLFW_PRESS) {
    KeyPressedEvent event{key};
    EventSystem::get().dispatch(event);
  } else if (action == GLFW_RELEASE) {
    KeyReleasedEvent event{key};
    EventSystem::get().dispatch(event);
  }
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
  auto newWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  newWindow->resized = true;
  newWindow->width = width;
  newWindow->height = height;
}

void Window::initialize() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  this->window =
      glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(this->window, this);
  glfwSetFramebufferSizeCallback(this->window, resizeCallback);
  glfwSetKeyCallback(this->window, keyEventCallback);
}

}  // namespace hep
