#include "application.hpp"

namespace alp {

Application::Application(const ApplicationConfig& config)
    : config{config},
      window{config.width, config.height, config.name},
      device{this->window},
      renderer{this->window, this->device},
      imGuiRenderSystem{this->window, this->device, this->renderer} {}

Application::~Application() { this->device.waitIdle(); };

void Application::run() {
  auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = startTime;

  EventSystem::get().addListener<KeyReleasedEvent>(
      std::bind(&Application::onEvent, this, std::placeholders::_1));

  this->sceneManager.onAttachActiveScene();

  while (this->isRunning) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    double deltaTime =
        std::chrono::duration<double>(newTime - currentTime).count();
    currentTime = newTime;

    // Attempt to start a new frame
    vk::CommandBuffer commandBuffer = this->renderer.beginFrame();
    if (commandBuffer != nullptr) {
      double elapsedTime =
          std::chrono::duration<double>(currentTime - startTime).count();

      vk::Extent2D extent = this->renderer.getCurrentFramebufferExtent();
      glm::vec2 extentVec2(static_cast<float>(extent.width),
                           static_cast<float>(extent.height));

      FrameInfo frameInfo{this->renderer.getFrameIndex(), elapsedTime,
                          deltaTime, extentVec2};

      /* ---- BEGIN UPDATE ----*/
      this->sceneManager.onUpdateActiveScene();
      /* ---- END UPDATE ----*/

      this->renderer.beginSwapChainRenderPass(commandBuffer);

      /* ---- BEGIN RENDER ---- */
      this->sceneManager.onRenderActiveScene();

      this->imGuiRenderSystem.beginFrame();
      this->sceneManager.onImGuiRenderActiveScene();
      this->imGuiRenderSystem.endFrame(commandBuffer);
      /* ---- END RENDER ---- */

      this->renderer.endSwapChainRenderPass(commandBuffer);
      this->renderer.endFrame();
    }
  }

  this->sceneManager.onDetachActiveScene();

  this->device.waitIdle();

  auto endTime = std::chrono::high_resolution_clock::now();
  double totalRuntime =
      std::chrono::duration<double>(endTime - startTime).count();
  log::info("Application ran for", totalRuntime, "s");
}

void Application::onEvent(KeyReleasedEvent& event) {
  if (event.getKeyCode() == Key::Escape) {
    log::info("Escape key pressed. Quiting...");
    this->isRunning = false;
  }
}

void Application::registerScene(std::unique_ptr<Scene> scene) {
  this->sceneManager.registerScene(std::move(scene));
}

}  // namespace alp
