/**
 * @file engine/vulkan/window.cpp
 * @author Caleb Burke
 * @date Jan 8, 2024
 */

#include "window.hpp"

#include "../util/logger.hpp"

namespace hep {

Window::Window(int width, int height, const std::string& name) : width{width}, height{height}, name{name} {
	initialize();
}

Window::~Window(){
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::create_surface(VkInstance instance, VkSurfaceKHR* surface){
	if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
		log(LEVEL::FATAL, "Failed to create VkSurfaceKHR.");
		throw std::exception();
	}
	log(LEVEL::TRACE, "Created VkSurfaceKHR.");
}

void Window::resize_callback(GLFWwindow* window, int width, int height){
	(void)window;
	(void)width;
	(void)height;
}

void Window::initialize(){
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
}

}	// namespace hep

