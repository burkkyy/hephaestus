#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "window.hpp"

namespace hep {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;

  bool isComplete() { return graphicsFamily.has_value(); }
};

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
      vk::DebugUtilsMessengerCreateInfoEXT& create_info);
  void setupDebugMessenger();

  bool checkValidationLayerSupport();
  std::vector<const char*> getRequiredExtensions();
  void createVulkanInstance();

  QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
  bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& device);
  void pickPhysicalDevice();

  vk::UniqueInstance instance;
  VkDebugUtilsMessengerEXT debug_messenger;

  Window& window;
  vk::SurfaceKHR surface;

  vk::PhysicalDevice physical_device = VK_NULL_HANDLE;

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
