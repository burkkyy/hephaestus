#pragma once

#include <vulkan/vulkan.h>

#include <vector>

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

 private:
  // Vulkan validation layer debugger funcs
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& create_info);
  void setupDebugMessenger();
  VkResult createDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger);
  void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                     VkDebugUtilsMessengerEXT debugMessenger,
                                     const VkAllocationCallbacks* pAllocator);

  bool checkValidationLayerSupport();
  std::vector<const char*> getRequiredExtensions();
  void createVulkanInstance();

  bool isPhysicalDeviceSuitable(VkPhysicalDevice physical_device);
  void pickPhysicalDevice();

  VkInstance instance;
  VkDebugUtilsMessengerEXT debug_messenger;
  Window& window;
  VkSurfaceKHR surface;
  VkPhysicalDevice physical_device = VK_NULL_HANDLE;

#ifdef NDEBUG
  const bool enable_validation_layers = false;
  const std::vector<const char*> enabled_layers;
#else
  const bool enable_validation_layers = true;
  const std::vector<const char*> enabled_layers = {
      "VK_LAYER_KHRONOS_validation"};
#endif
  const std::vector<const char*> enabled_extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace hep
