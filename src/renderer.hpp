#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "swapchain.hpp"
#include "util/types.hpp"
#include "window.hpp"

namespace hep {

class Renderer {
 public:
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  Renderer(Window& window, Device& device);
  ~Renderer();

  vk::RenderPass getSwapChainRenderPass() const {
    assert(this->swapchain != nullptr);
    return this->swapchain->getRenderPass();
  }

  size_t getSwapChainImageCount() const {
    return this->swapchain->imageCount();
  }

  vk::CommandBuffer getCurrentCommandBuffer() const {
    assert(this->isFrameStarted &&
           "Cannot get command buffer when frame not in progress");
    return this->commandBuffers.at(currentFrameIndex);
  }

  u32 getFrameIndex() const {
    assert(isFrameStarted &&
           "Cannot get frame index when frame not in progress");
    return static_cast<u32>(currentFrameIndex);
  }

  float getAspectRatio() const { return this->swapchain->extentAspectRatio(); }
  vk::Extent2D getCurrentFramebufferExtent() const {
    return this->swapchain->getExtent();
  }

  vk::CommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(vk::CommandBuffer commandBuffer);
  void endSwapChainRenderPass(vk::CommandBuffer commandBuffer);

  void populateImGuiInitInfo(ImGui_ImplVulkan_InitInfo& initInfo);

 private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapchain();

  Window& window;
  Device& device;
  std::unique_ptr<Swapchain> swapchain;
  std::vector<vk::CommandBuffer> commandBuffers;

  u32 currentImageIndex;
  int currentFrameIndex = 0;
  bool isFrameStarted = false;
};

}  // namespace hep
