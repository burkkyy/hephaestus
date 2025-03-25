#include "hephaestus/engine.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <chrono>
#include <memory>
#include <vulkan/vulkan.hpp>

#include "basic_render_system.hpp"
#include "device.hpp"
#include "events/event.hpp"
#include "events/key_event.hpp"
#include "frame_info.hpp"
#include "renderer.hpp"
#include "ui/ui_system.hpp"
#include "util/logger.hpp"
#include "window.hpp"

namespace hep {

static vk::DescriptorPool TempDescriptorPool = VK_NULL_HANDLE;

class Engine::Impl {
 public:
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;

  Impl()
      : window{WINDOW_WIDTH, WINDOW_HEIGHT},
        device{this->window},
        renderer{this->window, this->device} {}

  ~Impl() {
    if (TempDescriptorPool != VK_NULL_HANDLE) {
      this->device.get()->destroyDescriptorPool(TempDescriptorPool);
      TempDescriptorPool = VK_NULL_HANDLE;
      log::info("destroyed descriptorPool");
    }
  };

  void run() {
    BasicRenderSystem basicRenderSystem{
        this->device, this->renderer.getSwapChainRenderPass()};

    auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = startTime;

    EventSystem::get().addListener<KeyReleasedEvent>(
        std::bind(&Impl::onEvent, this, std::placeholders::_1));

    {
      /**
       * ripped from imgui glfw_vulkan example
       *
       * currently descriptor sets, pool etc is not implementated so using this
       * quick fix for imgui
       */
      vk::DescriptorPoolSize pool_sizes[] = {
          {vk::DescriptorType::eCombinedImageSampler,
           IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
      };
      vk::DescriptorPoolCreateInfo pool_info = {};
      pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
      pool_info.maxSets = 0;
      for (vk::DescriptorPoolSize& pool_size : pool_sizes)
        pool_info.maxSets += pool_size.descriptorCount;
      pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
      pool_info.pPoolSizes = pool_sizes;

      vk::Result result = this->device.get()->createDescriptorPool(
          &pool_info, nullptr, &TempDescriptorPool);

      if (result != vk::Result::eSuccess) {
        log::fatal("failed to create destriptor pool");
        throw std::runtime_error("failed to create descriptor pool");
      }

      log::info("created descriptorPool with pool size: ", pool_info.maxSets);
    }

    UISystem ui{this->window, this->device, this->renderer, TempDescriptorPool};

    // Setup systems
    ui.setup();

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

        // Update systems
        ui.update(frameInfo);

        this->renderer.beginSwapChainRenderPass(commandBuffer);

        // Render systems
        basicRenderSystem.render(commandBuffer, frameInfo);
        ui.render(commandBuffer);

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
