#pragma once

#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "window.hpp"

namespace hep {

class Renderer {
 public:
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  Renderer(Window& window, Device& device);
  ~Renderer();

  vk::RenderPass getSwapChainRenderPass() const {
    return this->swapchain.getRenderPass();
  }

 private:
  void createCommandBuffers();
  void freeCommandBuffers();

  Window& window;
  Device& device;
  Swapchain swapchain;
  std::vector<vk::CommandBuffer> commandBuffers;

  // temp
  Pipeline pipeline;
};

}  // namespace hep
