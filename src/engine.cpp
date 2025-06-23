#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <chrono>
#include <hephaestus>  // SDK
#include <memory>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "event.hpp"
#include "input/key_event.hpp"
#include "rendering/buffer.hpp"
#include "rendering/core/device.hpp"
#include "rendering/core/window.hpp"
#include "rendering/descriptors/descriptor_pool.hpp"
#include "rendering/descriptors/descriptor_set_layout.hpp"
#include "rendering/descriptors/descriptor_writer.hpp"
#include "rendering/frame_info.hpp"
#include "rendering/renderer.hpp"
#include "systems/basic_render_system.hpp"
#include "systems/shader_art_render_system.hpp"
#include "ui/ui_system.hpp"
#include "util/logger.hpp"

// UI componenets
#include "ui/components/debug_overlay.hpp"
#include "ui/components/shader_art.hpp"

namespace hep {

struct GlobalUbo {
  glm::mat4 projectionView{1.0f};
};

class Engine::Impl {
 public:
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;

  explicit Impl(const ApplicationConfig& config)
      : window{config.width, config.height, config.name},
        device{this->window},
        renderer{this->window, this->device} {
    // NOTE: pool size is being hardcoded here
    this->imguiDescriptorPool =
        DescriptorPool::Builder(this->device)
            .addPoolSize(vk::DescriptorType::eCombinedImageSampler,
                         IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 5)
            .setPoolFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 5)
            .build();

    this->globalDescriptorPool =
        DescriptorPool::Builder(this->device)
            .addPoolSize(vk::DescriptorType::eUniformBuffer,
                         Swapchain::MAX_FRAMES_IN_FLIGHT)
            .setPoolFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
            .build();
  }

  ~Impl() = default;

  void run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(
        Swapchain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < uboBuffers.size(); i++) {
      uboBuffers[i] =
          std::make_unique<Buffer>(this->device, sizeof(GlobalUbo), 1,
                                   vk::BufferUsageFlagBits::eUniformBuffer,
                                   vk::MemoryPropertyFlagBits::eHostVisible);

      uboBuffers[i]->map();
    }

    auto globalDescriptorSetLayout =
        DescriptorSetLayout::Builder(this->device)
            .addBinding(0, vk::DescriptorType::eUniformBuffer,
                        vk::ShaderStageFlagBits::eVertex)
            .build();

    std::vector<vk::DescriptorSet> globalDescriptorSets(
        Swapchain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < globalDescriptorSets.size(); i++) {
      auto bufferInfo = uboBuffers.at(i)->descriptorInfo();

      DescriptorWriter(*globalDescriptorSetLayout, *this->globalDescriptorPool)
          .writeBuffer(0, &bufferInfo)
          .build(globalDescriptorSets.at(i));
    }

    BasicRenderSystem basicRenderSystem{
        this->device, this->renderer.getSwapChainRenderPass()};

    auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = startTime;

    EventSystem::get().addListener<KeyReleasedEvent>(
        std::bind(&Impl::onEvent, this, std::placeholders::_1));

    this->ui = UISystem::Builder(this->window, this->device, this->renderer,
                                 this->imguiDescriptorPool->get())
                   .darkTheme()
                   .setDocking(true)
                   .setKeyboard(true)
                   .addComponent(std::make_unique<DebugOverlay>())
                   .build();

    // Setup systems
    this->ui->setup();
    // basicRenderSystem.setup();

    ShaderArtRenderSystem artRenderSystem{this->device, {500, 500}};
    ShaderArtWindow shaderArtWindow;
    shaderArtWindow.setup(artRenderSystem.getImageTextureID());

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

        // Update
        GlobalUbo ubo{};
        ubo.projectionView = glm::mat4{1.0f};
        uboBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameInfo.frameIndex]->flush();

        this->ui->update(frameInfo);

        artRenderSystem.render(commandBuffer, frameInfo);

        // Render
        this->renderer.beginSwapChainRenderPass(commandBuffer);

        shaderArtWindow.updateTextureId(artRenderSystem.getImageTextureID());
        shaderArtWindow.draw(frameInfo);
        basicRenderSystem.render(commandBuffer, frameInfo);
        this->ui->render(commandBuffer);

        this->renderer.endSwapChainRenderPass(commandBuffer);
        this->renderer.endFrame();

        // Post Render
        shaderArtWindow.postDraw();
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

  std::unique_ptr<DescriptorPool> imguiDescriptorPool{};
  std::unique_ptr<DescriptorPool> globalDescriptorPool{};

  std::unique_ptr<UISystem> ui{};

  bool isRunning = true;
};

Engine::Engine(const ApplicationConfig& config)
    : pImpl(std::make_unique<Impl>(config)) {}

Engine::~Engine() = default;

void Engine::run() { this->pImpl->run(); }

}  // namespace hep
