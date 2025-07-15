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

class SceneRenderSystem {
 public:
  SceneRenderSystem(const SceneRenderSystem&) = delete;
  SceneRenderSystem& operator=(const SceneRenderSystem&) = delete;

  SceneRenderSystem(Device& device, vk::RenderPass renderPass);
  ~SceneRenderSystem();

  void render(vk::CommandBuffer commandBuffer, FrameInfo frameInfo);

 private:
  void createPipelineLayout();
  void createPipeline(vk::RenderPass renderPass);

  void createQuadModel();

  Device& device;
  Pipeline pipeline;
  vk::PipelineLayout pipelineLayout;

  std::unique_ptr<Model> quadModel;
};

}  // namespace alp
