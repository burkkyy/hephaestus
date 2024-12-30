#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "window.hpp"

namespace hep {

class Device {
public:
  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;
  Device(Device&&) = delete;
  Device& operator=(Device&&) = delete;

  Device(Window& window);
	~Device();

  // Vulkan validation layer debugger funcs
  void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
	void setup_debug_messenger();
	VkResult create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

private:
  bool check_validation_layer_support();
  std::vector<const char*> get_required_extensions();
  void create_vulkan_instance();

  VkInstance instance;
  VkDebugUtilsMessengerEXT debug_messenger;
  Window& window;

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

}	// namespace hep
