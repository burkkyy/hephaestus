#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "util/types.hpp"

namespace hep {

class Swapchain {
 public:
  Swapchain(const Swapchain&) = delete;
  Swapchain& operator=(const Swapchain&) = delete;

  Swapchain(Device& device, vk::Extent2D extent);
  ~Swapchain();

  vk::Framebuffer getFrameBuffer(int index) {
    return this->framebuffers.at(index);
  }

  vk::RenderPass getRenderPass() const { return this->renderPass; }

  vk::ImageView getImageView(int index) { return this->imageViews.at(index); }

  size_t imageCount() { return this->images.size(); }

  vk::Format getImageFormat() { return this->imageFormat; }

  VkExtent2D getExtent() { return this->extent; }

  u32 width() { return this->extent.width; }

  u32 height() { return this->extent.height; }

 private:
  void setDefaultCreateInfo();
  void createSwapchain();
  void createImageViews();
  void createRenderPass();
  void createFramebuffers();

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
  std::vector<vk::Framebuffer> framebuffers;
};

}  // namespace hep
