/**
 * @file engine/vulkan/device.hpp
 * @author Caleb Burke
 * @date Jan 14th, 2024
 */

#pragma once

#include "window.hpp"
#include "util/util.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

namespace hep {
namespace vul {

/**
 * @brief Helper Struct 
 */
struct SwapChainSupportDetails {
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
    VkSurfaceCapabilitiesKHR capabilities;
};

/**
 * @brief Helper Struct
 */
struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;

    bool filled(){
        return graphics.has_value() && present.has_value();
    }
};

/**
 * @brief Creates interface with vulkan device
 */
class Device {
public:
    // Prevents copying and moving
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(Device&&) = delete;
	
	Device(Window& window);

	~Device();

    VkDevice get_device() const { return device; }
    VkSurfaceKHR get_surface() const { return surface; }
    VkQueue get_graphics_queue() const { return graphics_queue; }
    VkQueue get_present_queue() const { return present_queue; }
    SwapChainSupportDetails get_swapchain_support(){ return query_swapchain_support(physical_device); }
    QueueFamilyIndices get_queue_indices(){ return find_queue_families(physical_device); }

private:
    void initialize();

    void create_vulkan_instance();
    bool check_validation_layer_support();
    std::vector<const char*> get_required_extensions();

	void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
	void setup_debug_messenger();
	VkResult create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void pick_physical_device();
    bool is_physical_device_suitable(VkPhysicalDevice physical_device);
    QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device);
	bool check_device_extension_support(VkPhysicalDevice physical_device);
	SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice physical_device);
    void create_logical_device();

    VkInstance instance;
    Window& window;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;

#ifdef NDEBUG
    const bool enable_validation_layers = false;
    const std::vector<const char*> enabled_layers;
#else
    const bool enable_validation_layers = true;
    const std::vector<const char*> enabled_layers = {"VK_LAYER_KHRONOS_validation"};
#endif
    const std::vector<const char*> enabled_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};

}	// namespace vul
}	// namespace hep

