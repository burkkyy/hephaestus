#include "scene_render_system.hpp"

#include "descriptors/descriptor_pool.hpp"
#include "shaders/quad_frag.h"
#include "shaders/quad_vert.h"
#include "util/logger.hpp"

namespace alp {

SceneRenderSystem::SceneRenderSystem(Device& device, vk::RenderPass renderPass)
    : device{device}, pipeline{device} {
  createPipelineLayout();
  createPipeline(renderPass);
  createQuadModel();

  auto descriptorPool = DescriptorPool::Builder(this->device)
                            .setMaxSets(1)
                            .addPoolSize(vk::DescriptorType::eStorageBuffer, 1)
                            .build();
}

SceneRenderSystem ::~SceneRenderSystem() {
  log::trace("destroyed vk::PipelineLayout");
  this->device.get()->destroyPipelineLayout(this->pipelineLayout);
}

void SceneRenderSystem::render(vk::CommandBuffer commandBuffer,
                               FrameInfo frameInfo) {
  // this->pipeline.bind(commandBuffer);

  // quad->bind(commandBuffer);

  // quad->draw(commandBuffer);
}

void SceneRenderSystem::createPipelineLayout() {
  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  try {
    this->pipelineLayout =
        this->device.get()->createPipelineLayout(pipelineLayoutInfo);
    log::trace("created vk::PipelineLayout");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::PipelineLayout");
    throw std::runtime_error("failed to create vk::PipelineLayout");
  }
}

void SceneRenderSystem::createPipeline(vk::RenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  this->pipeline.create(quad_vert_spv, quad_vert_spv_len, quad_frag_spv,
                        quad_frag_spv_len, this->pipelineLayout, renderPass);
}

void SceneRenderSystem::createQuadModel() {
  Model::Builder quadBuilder{};
  quadBuilder.vertices = {{{-1.0f, -1.0f}, {1, 0, 0}},
                          {{-1.0f, 1.0f}, {1, 0, 0}},
                          {{1.0f, 1.0f}, {0, 1, 0}},
                          {{1.0f, -1.0f}, {0, 0, 1}}};
  quadBuilder.indices = {0, 1, 2, 0, 2, 3};

  this->quadModel = std::make_unique<Model>(this->device, quadBuilder);
}

}  // namespace alp
