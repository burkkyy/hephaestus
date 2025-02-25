#include "engine.hpp"

#include "render_system.hpp"
#include "util/logger.hpp"

namespace hep {

Engine::Engine()
    : window{WINDOW_WIDTH, WINDOW_HEIGHT},
      device{this->window},
      renderer{this->window, this->device} {}

Engine::~Engine() {}

void Engine::run() {
  RenderSystem renderSystem{this->device,
                            this->renderer.getSwapChainRenderPass()};

  while (!this->window.shouldClose()) {
    glfwPollEvents();

    vk::CommandBuffer commandBuffer = this->renderer.beginFrame();
    if (commandBuffer != nullptr) {
      this->renderer.beginSwapChainRenderPass(commandBuffer);

      vk::Extent2D extent = this->renderer.getCurrentFramebufferExtent();
      glm::vec2 extentVec2(static_cast<float>(extent.width),
                           static_cast<float>(extent.height));

      renderSystem.render(commandBuffer, extentVec2);

      this->renderer.endSwapChainRenderPass(commandBuffer);
      this->renderer.endFrame();
    }
  }

  this->device.waitIdle();
}

}  // namespace hep
