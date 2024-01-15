/**
 * @file engine/vulkan/window.hpp
 * @author Caleb Burke
 * @date Jan 8, 2024
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace hep {

class Window {
public:
	// Prevents Copying
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(int width, int height, const std::string& name);
	~Window();

	void create_surface(VkInstance instance, VkSurfaceKHR* surface);

private:
	static void resize_callback(GLFWwindow* window, int width, int height);
	void initialize();

	int width, height;
	std::string name;
	GLFWwindow* window;
};

}	// namespace hep

