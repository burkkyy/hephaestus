/**
 * @file engine/vulkan/swapchain.hpp
 * @author Caleb Burke
 * @date Jan 15, 2024
 */

#pragma once

#include "device.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace hep {
namespace vul {

class Swapchain {
public:
	// Prevents copying
	Swapchain(const Swapchain&) = delete;
	Swapchain& operator=(const Swapchain&) = delete;

	Swapchain(Device& device, VkExtent2D extent);
	~Swapchain();

    VkRenderPass get_render_pass() const { return render_pass; };

private:
	void initialize();
	void create_swapchain();
	void default_config(VkSwapchainCreateInfoKHR& create_info);
	
    VkSurfaceFormatKHR get_surface_format();
    VkPresentModeKHR get_present_mode();
    VkExtent2D get_extent();
    u32 get_image_count();

    void create_image_views();
    void create_render_pass();

	Device& device;
    SwapChainSupportDetails swapchain_support;
	VkExtent2D extent;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> images;
    VkFormat image_format;
    VkExtent2D image_extent;
    std::vector<VkImageView> image_views;
    VkRenderPass render_pass;
};

}	// namespace vul
}	// namespace hep

