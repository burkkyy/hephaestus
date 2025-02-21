#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "util/types.hpp"

namespace hep {

class Swapchain {
 public:
  static constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;

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

  vk::Result acquireNextImage(u32* imageIndex);
  vk::Result submitCommandBuffers(const vk::CommandBuffer* buffers,
                                  u32* imageIndex);

 private:
  void initialize();
  void setDefaultCreateInfo();
  void createSwapchain();
  void createImageViews();
  void createRenderPass();
  void createFramebuffers();
  void createSyncObjects();

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

  std::vector<vk::Semaphore> imageAvailableSemaphores;
  std::vector<vk::Semaphore> renderFinishedSemaphores;
  std::vector<vk::Fence> inFlightFences;
  std::vector<vk::Fence> imagesInFlight;

  size_t currentFrame = 0;
};

}  // namespace hep
