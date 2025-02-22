#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "util/types.hpp"
#include "window.hpp"

namespace hep {

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

struct SwapchainSupportDetails {
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> presentModes;
  vk::SurfaceCapabilitiesKHR capabilities;
};

class Device {
 public:
  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;
  Device(Device&&) = delete;
  Device& operator=(Device&&) = delete;

  Device(Window& window);
  ~Device();

  const vk::Device* get() const { return &this->device.get(); }
  vk::CommandPool getCommandPool() { return this->commandPool; }
  vk::SurfaceKHR getSurface() const { return surface; }
  vk::Queue getGraphicsQueue() const { return graphicsQueue; }
  vk::Queue getPresentQueue() const { return presentQueue; }

  QueueFamilyIndices getQueueIndices() {
    return findQueueFamilies(this->physicalDevice);
  }

  SwapchainSupportDetails getSwapchainSupport() {
    return querySwapchainSupport(this->physicalDevice);
  }

  u32 findMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties);

  void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                    vk::MemoryPropertyFlags properties, vk::Buffer& buffer,
                    vk::DeviceMemory& bufferMemory);

 private:
  void setupDebugMessenger();

  bool checkValidationLayerSupport();
  std::vector<const char*> getRequiredExtensions();
  void createVulkanInstance();

  QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
  bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
  bool isPhysicalDeviceSuitable(const vk::PhysicalDevice& device);
  void pickPhysicalDevice();

  SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice device);

  void createLogicalDevice();
  void createCommandPool();

  vk::UniqueInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  Window& window;
  vk::SurfaceKHR surface;

  vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
  vk::UniqueDevice device;

  vk::Queue graphicsQueue;
  vk::Queue presentQueue;

  vk::CommandPool commandPool;

#ifdef NDEBUG
  const bool enableValidationLayers = false;
  const std::vector<const char*> enabledLayers;
#else
  const bool enableValidationLayers = true;
  const std::vector<const char*> enabledLayers = {
      "VK_LAYER_KHRONOS_validation"};
#endif
  const std::vector<const char*> enabledExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

}  // namespace hep
