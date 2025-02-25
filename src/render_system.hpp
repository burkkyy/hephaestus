#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "model.hpp"
#include "pipeline.hpp"

namespace hep {

/*
 Still considering making this a base class for all other render systems.
 So I can create a set of render systems to update/render in batches

 Currently there isn't enough render systems to make this "worth it"
*/
class RenderSystem {
 public:
  RenderSystem(const RenderSystem&) = delete;
  RenderSystem& operator=(const RenderSystem&) = delete;

  RenderSystem(Device& device, vk::RenderPass renderPass);
  ~RenderSystem();

  void render(vk::CommandBuffer commandBuffer);

 private:
  void createPipelineLayout();
  void createPipeline(vk::RenderPass renderPass);

  Device& device;
  Pipeline pipeline;
  vk::PipelineLayout pipelineLayout;
  std::unique_ptr<Model> model;
};

}  // namespace hep
