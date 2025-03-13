#include "engine.hpp"

#include <chrono>

#include "frame_info.hpp"
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

  auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = startTime;

  EventSystem::get().addListener<KeyReleasedEvent>(
      std::bind(&Engine::onEvent, this, std::placeholders::_1));

  while (this->isRunning && !this->window.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    double deltaTime =
        std::chrono::duration<double, std::chrono::seconds::period>(newTime -
                                                                    currentTime)
            .count();
    currentTime = newTime;

    vk::CommandBuffer commandBuffer = this->renderer.beginFrame();
    if (commandBuffer != nullptr) {
      double elapsedTime =
          std::chrono::duration<double>(currentTime - startTime).count();

      vk::Extent2D extent = this->renderer.getCurrentFramebufferExtent();
      glm::vec2 extentVec2(static_cast<float>(extent.width),
                           static_cast<float>(extent.height));

      FrameInfo frameInfo{commandBuffer, this->renderer.getFrameIndex(),
                          elapsedTime, deltaTime, extentVec2};

      this->renderer.beginSwapChainRenderPass(commandBuffer);

      renderSystem.render(commandBuffer, frameInfo);

      this->renderer.endSwapChainRenderPass(commandBuffer);
      this->renderer.endFrame();
    }
  }

  this->device.waitIdle();
}

void Engine::onEvent(KeyReleasedEvent& event) {
  if (event.getKeyCode() == Key::Escape) {
    log::info("Escape key pressed. Quiting...");
    this->isRunning = false;
  }
}

}  // namespace hep
