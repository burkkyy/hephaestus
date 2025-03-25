#include "basic_render_system.hpp"

#include "util/logger.hpp"

namespace hep {

BasicRenderSystem::BasicRenderSystem(Device& device, vk::RenderPass renderPass)
    : device{device}, pipeline{device} {
  createPipelineLayout();
  createPipeline(renderPass);

  Model::Builder firstTriangleBuilder{};
  firstTriangleBuilder.vertices = {{{-0.5f, 0.5f}, {1, 0, 0}},
                                   {{0.5f, 0.5f}, {0, 1, 0}},
                                   {{0.0f, -0.5f}, {0, 0, 1}}};

  this->model = std::make_unique<Model>(this->device, firstTriangleBuilder);

  Model::Builder secondTriangleBuilder{};
  secondTriangleBuilder.vertices = {{{-1.0f, -1.0f}, {1, 0, 0}},
                                    {{-1.0f, 1.0f}, {0, 1, 0}},
                                    {{1.0f, 1.0f}, {0, 0, 1}}};

  this->triangle1 =
      std::make_unique<Model>(this->device, secondTriangleBuilder);

  Model::Builder thirdTriangleBuilder{};
  thirdTriangleBuilder.vertices = {{{-1.0f, -1.0f}, {1, 0, 0}},
                                   {{1.0f, 1.0f}, {0, 1, 0}},
                                   {{1.0f, -1.0f}, {0, 0, 1}}};

  this->triangle2 = std::make_unique<Model>(this->device, thirdTriangleBuilder);

  pushConstant.transform = {{1.0f, 0.0f, 0.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f, 0.0f},
                            {0.0f, 0.0f, 1.0f, 0.0f},
                            {0.0f, 0.0f, 0.0f, 1.0f}};
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

  // Triangle 1
  triangle1->bind(commandBuffer);

  pushConstant.color = {1.0f, 0.0f, 0.0f, 1.0f};
  commandBuffer.pushConstants(
      this->pipelineLayout,
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
      sizeof(PushConstantData), &pushConstant);

  triangle1->draw(commandBuffer);

  // Triangle 2
  triangle2->bind(commandBuffer);

  pushConstant.color = {0.0f, 1.0f, 0.0f, 1.0f};

  commandBuffer.pushConstants(
      this->pipelineLayout,
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
      sizeof(PushConstantData), &pushConstant);

  triangle2->draw(commandBuffer);

  // this->model->bind(commandBuffer);

  // frameCount++;
  // if (frameCount % 1000 == 0) {
  //   frameCount = 0;

  //   // translation
  //   pushConstant.transform[3].x += 0.1f;
  //   pushConstant.transform[3].y += 0.1f;
  // }

  // commandBuffer.pushConstants(this->pipelineLayout,
  //                             vk::ShaderStageFlagBits::eVertex, 0,
  //                             sizeof(PushConstantData), &pushConstant);

  // this->model->draw(commandBuffer);
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
