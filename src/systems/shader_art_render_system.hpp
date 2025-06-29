#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "core/device.hpp"
#include "core/pipeline.hpp"
#include "descriptors/descriptor_pool.hpp"
#include "descriptors/descriptor_set_layout.hpp"
#include "descriptors/descriptor_writer.hpp"
#include "frame.hpp"
#include "frame_info.hpp"
#include "model.hpp"
#include "ui/components/shader_art.hpp"

namespace hep {

/*
 Still considering making this a base class for all other render systems.
 So I can create a set of render systems to update/render in batches

 Currently there isn't enough render systems to make this "worth it"
*/
class ShaderArtRenderSystem {
 public:
  ShaderArtRenderSystem(const ShaderArtRenderSystem&) = delete;
  ShaderArtRenderSystem& operator=(const ShaderArtRenderSystem&) = delete;

  struct PushConstantData {
    glm::mat4 transform;
    glm::vec4 color;
    glm::vec4 data;
  };

  ShaderArtRenderSystem(Device& device, vk::Extent2D extent);
  ~ShaderArtRenderSystem();

  void render(vk::CommandBuffer commandBuffer, FrameInfo frameInfo);

  vk::DescriptorSet getImageDescriptorSet();
  ImTextureID getImageTextureID();

  void onWindowResizeEvent(ShaderArtWindowResizeEvent& event);

 private:
  void createPipelineLayout();
  void createPipeline();
  void createSampler();
  // void createDescriptorResources();
  void createImGuiTexture();

  Device& device;
  vk::Extent2D extent;
  std::unique_ptr<Frame> frame;

  Pipeline pipeline;
  vk::PipelineLayout pipelineLayout;

  vk::Sampler sampler;
  VkDescriptorSet imguiDescriptorSet;

  std::unique_ptr<Model> quad;
  PushConstantData pushConstant;
};

}  // namespace hep
