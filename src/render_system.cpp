#include "render_system.hpp"

#include "util/logger.hpp"

namespace hep {

RenderSystem::RenderSystem(Device& device, vk::RenderPass renderPass)
    : device{device}, pipeline{device} {
  createPipelineLayout();
  createPipeline(renderPass);

  std::vector<Model::Vertex> vertices{{{-0.5f, 0.5f}, {1, 0, 0}},
                                      {{0.5f, 0.5f}, {0, 1, 0}},
                                      {{0.0f, -0.5f}, {0, 0, 1}}};
  this->model = std::make_unique<Model>(this->device, vertices);
}

RenderSystem ::~RenderSystem() {
  log::verbose("destroyed vk::PipelineLayout");
  this->device.get()->destroyPipelineLayout(this->pipelineLayout);
}

void RenderSystem::render(vk::CommandBuffer commandBuffer) {
  this->pipeline.bind(commandBuffer);
  this->model->bind(commandBuffer);
  this->model->draw(commandBuffer);
}

void RenderSystem::createPipelineLayout() {
  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  try {
    this->pipelineLayout =
        this->device.get()->createPipelineLayout(pipelineLayoutInfo);
    log::verbose("created vk::PipelineLayout");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::PipelineLayout");
    throw std::runtime_error("failed to create vk::PipelineLayout");
  }
}

void RenderSystem::createPipeline(vk::RenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  this->pipeline.create("shaders/triangle.vert.spv",
                        "shaders/triangle.frag.spv", this->pipelineLayout,
                        renderPass);
}

}  // namespace hep
