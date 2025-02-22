#include "engine.hpp"

#include "util/logger.hpp"

namespace hep {

Engine::Engine()
    : window{WINDOW_WIDTH, WINDOW_HEIGHT},
      device{this->window},
      renderer{this->window, this->device},
      pipeline{device} {
  std::vector<Model::Vertex> vertices{{{-0.5f, 0.5f}, {1, 0, 0}},
                                      {{0.5f, 0.5f}, {0, 1, 0}},
                                      {{0.0f, -0.5f}, {0, 0, 1}}};
  // sierpinski(vertices, 5, {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, -0.5f});
  this->model = std::make_unique<Model>(this->device, vertices);

  createPipelineLayout();
  createPipeline(this->renderer.getSwapChainRenderPass());
}

void Engine::sierpinski(std::vector<Model::Vertex>& vertices, int depth,
                        glm::vec2 left, glm::vec2 right, glm::vec2 top) {
  if (depth <= 0) {
    vertices.push_back({top});
    vertices.push_back({right});
    vertices.push_back({left});
  } else {
    auto leftTop = 0.5f * (left + top);
    auto rightTop = 0.5f * (right + top);
    auto leftRight = 0.5f * (left + right);
    sierpinski(vertices, depth - 1, left, leftRight, leftTop);
    sierpinski(vertices, depth - 1, leftRight, right, rightTop);
    sierpinski(vertices, depth - 1, leftTop, rightTop, top);
  }
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
      this->model->bind(commandBuffer);
      this->model->draw(commandBuffer);

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
