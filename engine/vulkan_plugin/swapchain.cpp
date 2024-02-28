/**
 * @file engine/vulkan/swapchain.cpp
 * @author Caleb Burke
 * @date Jan 16, 2024
 */

#include "swapchain.hpp"
#include "device.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>

namespace hep {
namespace vul {

Swapchain::Swapchain(Device& device, VkExtent2D extent) : device{device}, extent{extent} {
	initialize();
}

Swapchain::~Swapchain(){
	vkDestroySwapchainKHR(device.get_device(), swapchain, nullptr);
	log(LEVEL::TRACE, "Destroyed VkSwapchainKHR.");
}

void Swapchain::initialize(){
    swapchain_support = device.get_swapchain_support();
	create_swapchain();
}

void Swapchain::create_swapchain(){
	VkSwapchainCreateInfoKHR create_info = {};
	default_config(create_info);
    
    // It only worked in this scope for some reason
    QueueFamilyIndices indices = device.get_queue_indices();
    if(indices.graphics != indices.present){
        u32 queue_family_indices[] = {indices.graphics.value(), indices.present.value()};
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }

    VkResult result = vkCreateSwapchainKHR(device.get_device(), &create_info, nullptr, &swapchain);
	if(result != VK_SUCCESS){
		log(LEVEL::FATAL, "Failed to create VkSwapchainKHR. Error code: ", result);
		throw std::exception();
	}
	log(LEVEL::TRACE, "Created VkSwapchainKHR.");
    
    u32 image_count = 0;
    vkGetSwapchainImagesKHR(device.get_device(), swapchain, &image_count, nullptr);
    images.resize(image_count);
    vkGetSwapchainImagesKHR(device.get_device(), swapchain, &image_count, images.data());

    // NOTE these may be null
    image_format = create_info.imageFormat;
    image_extent = create_info.imageExtent;
}

void Swapchain::default_config(VkSwapchainCreateInfoKHR& create_info){
	/*
	https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSwapchainCreateInfoKHR.html
	
	typedef struct VkSwapchainCreateInfoKHR {
    	VkStructureType                  sType;
    	const void*                      pNext;
    	VkSwapchainCreateFlagsKHR        flags;
    	VkSurfaceKHR                     surface;
    	uint32_t                         minImageCount;
    	VkFormat                         imageFormat;
    	VkColorSpaceKHR                  imageColorSpace;
    	VkExtent2D                       imageExtent;
    	uint32_t                         imageArrayLayers;
    	VkImageUsageFlags                imageUsage;
    	VkSharingMode                    imageSharingMode;
    	uint32_t                         queueFamilyIndexCount;
    	const uint32_t*                  pQueueFamilyIndices;
    	VkSurfaceTransformFlagBitsKHR    preTransform;
    	VkCompositeAlphaFlagBitsKHR      compositeAlpha;
    	VkPresentModeKHR                 presentMode;
    	VkBool32                         clipped;
    	VkSwapchainKHR                   oldSwapchain;
	} VkSwapchainCreateInfoKHR; 
	*/
    
    VkSurfaceFormatKHR format = get_surface_format();

	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.pNext = nullptr;
	create_info.flags = 0;
	create_info.surface = device.get_surface();
	create_info.minImageCount = get_image_count();
	create_info.imageFormat = format.format;
	create_info.imageColorSpace = format.colorSpace;  // Use vkGetPhysicalDeviceSurfaceFormatsKHR to find out
	create_info.imageExtent = get_extent();
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;

    create_info.preTransform = swapchain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = get_present_mode();
    create_info.clipped = 0;
    create_info.oldSwapchain = VK_NULL_HANDLE;  // TODO Impl tracking old swapchain object for optimization
}

VkSurfaceFormatKHR Swapchain::get_surface_format(){
    for(const auto& available_format : swapchain_support.formats){
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return available_format;
        }
    }
    log(LEVEL::TRACE, "No available surface formats, choosing default.");
    return swapchain_support.formats[0];
}

VkPresentModeKHR Swapchain::get_present_mode(){
    for(const auto& present_mode : swapchain_support.present_modes){
        if(present_mode == VK_PRESENT_MODE_MAILBOX_KHR){
            return present_mode;
        }
    }
    log(LEVEL::TRACE, "Present mode MAILBOX not available, defaulting to FIFO.");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::get_extent(){
    if (swapchain_support.capabilities.currentExtent.width != 0xFFFFFFFF) {
        return swapchain_support.capabilities.currentExtent;
    } else {
        u32 min_width = swapchain_support.capabilities.minImageExtent.width;
        u32 max_width = swapchain_support.capabilities.maxImageExtent.width;
        u32 min_height = swapchain_support.capabilities.minImageExtent.height;
        u32 max_height = swapchain_support.capabilities.maxImageExtent.height;
        
        VkExtent2D choosen_extent = {};
        choosen_extent.width = std::clamp(extent.width, min_width, max_width);
        choosen_extent.height = std::clamp(extent.height, min_height, max_height);
        return choosen_extent;
    }
}

u32 Swapchain::get_image_count(){
    u32 image_count = swapchain_support.capabilities.minImageCount + 1;
    if(swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount){
        image_count = swapchain_support.capabilities.maxImageCount;
    }
    return image_count;
}

void Swapchain::create_image_views(){
    image_views.resize(images.size());
    
    for(VkImage image : images){
        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = image;
    }
}

}	// namespace vul
}	// namespace hep

