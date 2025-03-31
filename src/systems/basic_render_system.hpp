#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "rendering/core/device.hpp"
#include "rendering/core/pipeline.hpp"
#include "rendering/frame_info.hpp"
#include "rendering/model.hpp"

namespace hep {

/*
 Still considering making this a base class for all other render systems.
 So I can create a set of render systems to update/render in batches

 Currently there isn't enough render systems to make this "worth it"
*/
class BasicRenderSystem {
 public:
  BasicRenderSystem(const BasicRenderSystem&) = delete;
  BasicRenderSystem& operator=(const BasicRenderSystem&) = delete;

  struct PushConstantData {
    glm::mat4 transform;
    glm::vec4 color;
    glm::vec4 data;
  };

  BasicRenderSystem(Device& device, vk::RenderPass renderPass);
  ~BasicRenderSystem();

  void render(vk::CommandBuffer commandBuffer, FrameInfo frameInfo);

 private:
  void createPipelineLayout();
  void createPipeline(vk::RenderPass renderPass);

  // temp
  PushConstantData pushConstant;
  u32 frameCount;

  Device& device;
  Pipeline pipeline;
  vk::PipelineLayout pipelineLayout;

  std::unique_ptr<Model> quad;
};

}  // namespace hep
