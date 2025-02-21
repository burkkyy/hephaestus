#include "engine.hpp"

#include "util/logger.hpp"

namespace hep {

Engine::Engine()
    : window{WINDOW_WIDTH, WINDOW_HEIGHT},
      device{this->window},
      renderer{this->window, this->device},
      pipeline{device} {
  createPipelineLayout();
  createPipeline(this->renderer.getSwapChainRenderPass());
}

Engine::~Engine() {
  log::verbose("destroyed vk::PipelineLayout");
  this->device.get()->destroyPipelineLayout(this->pipelineLayout);
}

void Engine::run() {
  while (!this->window.shouldClose()) {
    glfwPollEvents();

    vk::CommandBuffer commandBuffer = this->renderer.beginFrame();
    if (commandBuffer != nullptr) {
      this->renderer.beginSwapChainRenderPass(commandBuffer);

      this->pipeline.bind(commandBuffer);
      commandBuffer.draw(3, 1, 0, 0);

      this->renderer.endSwapChainRenderPass(commandBuffer);
      this->renderer.endFrame();
    }
  }

  this->device.get()->waitIdle();
}

void Engine::createPipelineLayout() {
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

void Engine::createPipeline(vk::RenderPass renderPass) {
  assert(pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  this->pipeline.create("shaders/triangle.vert.spv",
                        "shaders/triangle.frag.spv", this->pipelineLayout,
                        renderPass);
}

void Engine::render() {}

}  // namespace hep
