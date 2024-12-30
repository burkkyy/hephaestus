#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "util/types.hpp"

namespace hep {

class Window {
public:
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(u32 width, u32 height, const std::string& name = "hephaestus");
	~Window();

	bool shouldClose(){ return glfwWindowShouldClose(window); }

	VkExtent2D getExtent(){ return {width, height}; }
	void createSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
	static void resizeCallback(GLFWwindow* window, u32 width, u32 height);
	void initialize();

	u32 width, height;
	std::string name;
	GLFWwindow* window;
};

}	// namespace hep
