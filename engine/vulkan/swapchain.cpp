/**
 * @file engine/vulkan/swapchain.cpp
 * @author Caleb Burke
 * @date Jan 16, 2024
 */

#include "swapchain.hpp"

#include "../util/util.hpp"

namespace hep {

Swapchain::Swapchain(Device& device, VkExtent2D extent) : device{device}, extent{extent} {
	initialize();
}

Swapchain::~Swapchain(){
	vkDestroySwapchainKHR(device.get_device(), swapchain, nullptr);
	log(LEVEL::TRACE, "Destroyed VkSwapchainKHR.");
}

void Swapchain::initialize(){
	create_swapchain();
}

void Swapchain::create_swapchain(){
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = device.get_surface();
	create_info.minImageCount = 4;
	create_info.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	create_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	// ^^ Use vkGetPhysicalDeviceSurfaceFormatsKHR to find out ^^
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

	VkResult result = vkCreateSwapchainKHR(device.get_device(), &create_info, nullptr, &swapchain);
	if(result != VK_SUCCESS){
		log(LEVEL::FATAL, "Failed to create VkSwapchainKHR. Error code: ", result);
		throw std::exception();
	}
	log(LEVEL::TRACE, "Created VkSwapchainKHR.");
}

}	// namespace hep

