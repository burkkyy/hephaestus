#include "application.hpp"

namespace hep {

Application::Application(const ApplicationConfig& config)
    : config{config},
      window{config.width, config.height, config.name},
      device{this->window},
      renderer{this->window, this->device} {
  this->imguiDescriptorPool =
      DescriptorPool::Builder(this->device)
          .addPoolSize(vk::DescriptorType::eCombinedImageSampler,
                       IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 5)
          .setPoolFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
          .setMaxSets(IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 5)
          .build();

  this->uiManager =
      UIManager::Builder(this->window, this->device, this->renderer,
                         imguiDescriptorPool->get())
          .darkTheme()
          .setDocking(true)
          .setKeyboard(true)
          .build();
};

Application::~Application() { this->device.waitIdle(); };

void Application::run() {
  auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = startTime;

  EventSystem::get().addListener<KeyReleasedEvent>(
      std::bind(&Application::onEvent, this, std::placeholders::_1));

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
      uiManager->updatePanels();
      /* ---- END UPDATE ----*/

      this->renderer.beginSwapChainRenderPass(commandBuffer);

      /* ---- BEGIN RENDER ---- */
      uiManager->renderPanels(commandBuffer);
      /* ---- END RENDER ---- */

      this->renderer.endSwapChainRenderPass(commandBuffer);
      this->renderer.endFrame();
    }
  }

  this->device.waitIdle();

  auto endTime = std::chrono::high_resolution_clock::now();
  double totalRuntime =
      std::chrono::duration<double>(endTime - startTime).count();
  log::info("Application ran for", totalRuntime, "s");
}

void Application::registerPanel(std::unique_ptr<Panel> panel) {
  uiManager->registerPanel(std::move(panel));
}

void Application::onEvent(KeyReleasedEvent& event) {
  if (event.getKeyCode() == Key::Escape) {
    log::info("Escape key pressed. Quiting...");
    this->isRunning = false;
  }
}

}  // namespace hep
