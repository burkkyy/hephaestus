#include "basic_render_system.hpp"

#include "util/logger.hpp"

namespace hep {

BasicRenderSystem::BasicRenderSystem(Device& device, vk::RenderPass renderPass)
    : device{device}, pipeline{device} {
  createPipelineLayout();
  createPipeline(renderPass);

  Model::Builder quadBuilder{};
  quadBuilder.vertices = {{{-1.0f, -1.0f}, {1, 0, 0}},
                          {{-1.0f, 1.0f}, {1, 0, 0}},
                          {{1.0f, 1.0f}, {0, 1, 0}},
                          {{1.0f, -1.0f}, {0, 0, 1}}};
  quadBuilder.indicies = {0, 1, 2, 0, 2, 3};

  this->quad = std::make_unique<Model>(this->device, quadBuilder);

  pushConstant.transform = glm::mat4(1.0f);
}

BasicRenderSystem ::~BasicRenderSystem() {
  log::trace("destroyed vk::PipelineLayout");
  this->device.get()->destroyPipelineLayout(this->pipelineLayout);
}

void BasicRenderSystem::render(vk::CommandBuffer commandBuffer,
                               FrameInfo frameInfo) {
  this->pipeline.bind(commandBuffer);

  pushConstant.data = {frameInfo.currentFramebufferExtent.x,
                       frameInfo.currentFramebufferExtent.y,
                       frameInfo.elapsedTime, 0.0f};

  quad->bind(commandBuffer);

  pushConstant.color = {1.0f, 0.0f, 0.0f, 1.0f};
  commandBuffer.pushConstants(
      this->pipelineLayout,
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
      sizeof(PushConstantData), &pushConstant);

  quad->draw(commandBuffer);
}

void BasicRenderSystem::createPipelineLayout() {
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

void BasicRenderSystem::createPipeline(vk::RenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  this->pipeline.create("shaders/triangle.vert.spv",
                        "shaders/triangle.frag.spv", this->pipelineLayout,
                        renderPass);
}

}  // namespace hep
