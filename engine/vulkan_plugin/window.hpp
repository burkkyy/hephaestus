/**
 * @file engine/vulkan/window.hpp
 * @author Caleb Burke
 * @date Jan 8, 2024
 */

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "util/util.hpp"

#include <string>

namespace hep {
namespace vul {

class Window {
public:
	// Prevents Copying
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(u32 width, u32 height, const std::string& name = "hephaestus");
	~Window();

	bool should_close(){ return glfwWindowShouldClose(window); }

	VkExtent2D get_extent(){ return {width, height}; }
	void create_surface(VkInstance instance, VkSurfaceKHR* surface);

private:
	static void resize_callback(GLFWwindow* window, u32 width, u32 height);
	void initialize();

	u32 width, height;
	std::string name;
	GLFWwindow* window;
};

}	// namepace vul
}	// namespace hep

