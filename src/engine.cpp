#include "hephaestus/engine.hpp"

#include <chrono>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "events/event.hpp"
#include "events/key_event.hpp"
#include "frame_info.hpp"
#include "render_system.hpp"
#include "renderer.hpp"
#include "util/logger.hpp"
#include "window.hpp"

namespace hep {

class Engine::Impl {
 public:
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;

  Impl()
      : window{WINDOW_WIDTH, WINDOW_HEIGHT},
        device{this->window},
        renderer{this->window, this->device} {}

  ~Impl() = default;

  void run() {
    RenderSystem renderSystem{this->device,
                              this->renderer.getSwapChainRenderPass()};

    auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = startTime;

    EventSystem::get().addListener<KeyReleasedEvent>(
        std::bind(&Impl::onEvent, this, std::placeholders::_1));

    while (this->isRunning && !this->window.shouldClose()) {
      glfwPollEvents();

      auto newTime = std::chrono::high_resolution_clock::now();
      double deltaTime =
          std::chrono::duration<double, std::chrono::seconds::period>(
              newTime - currentTime)
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

  void onEvent(KeyReleasedEvent& event) {
    if (event.getKeyCode() == Key::Escape) {
      log::info("Escape key pressed. Quiting...");
      this->isRunning = false;
    }
  }

 private:
  Window window;
  Device device;
  Renderer renderer;

  bool isRunning = true;
};

Engine::Engine() : pImpl(std::make_unique<Impl>()) {}

Engine::~Engine() = default;

void Engine::run() { this->pImpl->run(); }

}  // namespace hep
