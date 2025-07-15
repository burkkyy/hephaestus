#include "quad_render_system.hpp"

#include "shaders/quad_frag.h"
#include "shaders/quad_vert.h"
#include "util/logger.hpp"

namespace alp {

QuadRenderSystem::QuadRenderSystem(Device& device, vk::RenderPass renderPass)
    : device{device}, pipeline{device} {
  createPipelineLayout();
  createPipeline(renderPass);

  pushConstant.transform = glm::mat4(1.0f);
}

QuadRenderSystem ::~QuadRenderSystem() {
  log::trace("destroyed vk::PipelineLayout");
  this->device.get()->destroyPipelineLayout(this->pipelineLayout);
}

void QuadRenderSystem::addQuad(glm::vec2 position, float width, float height) {
  Model::Builder quadBuilder{};
  quadBuilder.vertices = {
      {{position.x, position.y}, {1, 0, 0}},
      {{position.x + width, position.y}, {1, 0, 0}},
      {{position.x, position.y + height}, {0, 1, 0}},
      {{position.x + width, position.y + height}, {0, 0, 1}}};
  quadBuilder.indices = {0, 1, 2, 1, 2, 3};

  std::unique_ptr<Model> quad =
      std::make_unique<Model>(this->device, quadBuilder);

  this->quads.push_back(std::move(quad));
}

void QuadRenderSystem::render(vk::CommandBuffer commandBuffer,
                              FrameInfo frameInfo) {
  assert(quads.size() != 0);
  this->pipeline.bind(commandBuffer);

  for (auto& quad : this->quads) {
    quad->bind(commandBuffer);

    pushConstant.color = {1.0f, 0.0f, 0.0f, 1.0f};
    commandBuffer.pushConstants(
        this->pipelineLayout,
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        0, sizeof(PushConstantData), &pushConstant);

    quad->draw(commandBuffer);
  }
}

void QuadRenderSystem::createPipelineLayout() {
  vk::PushConstantRange pushConstantRange{};

  pushConstantRange.stageFlags =
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PushConstantData);

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  try {
    this->pipelineLayout =
        this->device.get()->createPipelineLayout(pipelineLayoutInfo);
    log::trace("created vk::PipelineLayout");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::PipelineLayout");
    throw std::runtime_error("failed to create vk::PipelineLayout");
  }
}

void QuadRenderSystem::createPipeline(vk::RenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  this->pipeline.create(quad_vert_spv, quad_vert_spv_len, quad_frag_spv,
                        quad_frag_spv_len, this->pipelineLayout, renderPass);
}

}  // namespace alp
