#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "device.hpp"

namespace hep {

class Swapchain {
 public:
  Swapchain(const Swapchain&) = delete;
  Swapchain& operator=(const Swapchain&) = delete;

  Swapchain(Device& device, vk::Extent2D extent);
  ~Swapchain();

  vk::RenderPass getRenderPass() const { return this->renderPass; }

 private:
  void setDefaultCreateInfo();
  void createSwapchain();
  void createImageViews();
  void createRenderPass();

  vk::SurfaceFormatKHR chooseSurfaceFormat(
      const std::vector<vk::SurfaceFormatKHR>& availableFormats);
  vk::PresentModeKHR choosePresentMode(
      const std::vector<vk::PresentModeKHR> availablePresentModes);
  vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

  Device& device;
  vk::SwapchainKHR swapchain;
  vk::SwapchainCreateInfoKHR swapchainCreateInfo;
  std::vector<vk::Image> images;
  vk::Format imageFormat;
  vk::Extent2D extent;
  std::vector<vk::ImageView> imageViews;

  vk::RenderPass renderPass;
};

}  // namespace hep
