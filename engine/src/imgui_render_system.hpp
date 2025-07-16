#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "descriptors/descriptor_pool.hpp"
#include "device.hpp"
#include "frame_info.hpp"
#include "panel.hpp"
#include "renderer.hpp"
#include "window.hpp"

namespace alp {

class ImGuiRenderSystem {
 public:
  ImGuiRenderSystem(const ImGuiRenderSystem&) = delete;
  ImGuiRenderSystem& operator=(const ImGuiRenderSystem&) = delete;

  ImGuiRenderSystem(Window& window, Device& device, Renderer& renderer);
  ~ImGuiRenderSystem();

  void beginFrame();
  void endFrame(vk::CommandBuffer commandBuffer);

 private:
  std::unique_ptr<alp::DescriptorPool> descriptorPool;
};

}  // namespace alp
