/**
 * @file src/window.hpp
 * @author Caleb Burke
 * @date Jan 8, 2024
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "util/types.hpp"

#include <string>

namespace hep {
namespace vul {

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

}	// namepace vul
}	// namespace hep
