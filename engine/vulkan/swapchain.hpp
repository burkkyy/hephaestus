/**
 * @file engine/vulkan/swapchain.hpp
 * @author Caleb Burke
 * @date Jan 15, 2024
 */

#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>

namespace hep {

class Swapchain {
public:
	// Prevents copying
	Swapchain(const Swapchain&) = delete;
	Swapchain& operator=(const Swapchain&) = delete;

	Swapchain(Device& device, VkExtent2D extent);
	~Swapchain();

private:
	void initialize();
	void create_swapchain();

	Device& device;
	VkExtent2D extent;
	VkSwapchainKHR swapchain;
};

}	// namespace hep

