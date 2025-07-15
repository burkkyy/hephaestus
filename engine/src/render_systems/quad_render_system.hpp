#pragma once

#include "device.hpp"
#include "frame_info.hpp"
#include "model.hpp"
#include "pipeline.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace alp {

class QuadRenderSystem {
 public:
  QuadRenderSystem(const QuadRenderSystem&) = delete;
  QuadRenderSystem& operator=(const QuadRenderSystem&) = delete;

  struct PushConstantData {
    glm::mat4 transform;
    glm::vec4 color;
  };

  QuadRenderSystem(Device& device, vk::RenderPass renderPass);
  ~QuadRenderSystem();

  void addQuad(glm::vec2 position, float width, float height);

  void render(vk::CommandBuffer commandBuffer, FrameInfo frameInfo);

 private:
  void createPipelineLayout();
  void createPipeline(vk::RenderPass renderPass);

  PushConstantData pushConstant;
  u32 frameCount;

  Device& device;
  Pipeline pipeline;
  vk::PipelineLayout pipelineLayout;

  std::vector<std::unique_ptr<Model>> quads;
};

}  // namespace alp
