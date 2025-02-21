#include "renderer.hpp"

#include "util/logger.hpp"

namespace hep {

Renderer::Renderer(Window& window, Device& device)
    : window{window}, device{device}, swapchain{device, window.getExtent()} {
  createCommandBuffers();
}

Renderer::~Renderer() {}

vk::CommandBuffer Renderer::beginFrame() {
  assert(!this->isFrameStarted &&
         "Can't call beginFrame while already in progress");

  vk::Result result = this->swapchain.acquireNextImage(&currentImageIndex);
  if (result == vk::Result::eErrorOutOfDateKHR) {
    throw std::runtime_error("swap chain KHR out of date");
  }

  if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
    throw std::runtime_error("failed to acquire swap chain image");
  }

  this->isFrameStarted = true;

  vk::CommandBuffer commandBuffer = getCurrentCommandBuffer();

  vk::CommandBufferBeginInfo beginInfo = {};
  beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

  try {
    commandBuffer.begin(beginInfo);
  } catch (const vk::SystemError& err) {
    log::fatal("failed to begin recording command buffer");
    throw std::runtime_error("failed to begin recording command buffer");
  }

  return commandBuffer;
}

void Renderer::endFrame() {
  assert(isFrameStarted &&
         "Can't call endFrame while frame is not in progress");

  vk::CommandBuffer commandBuffer = getCurrentCommandBuffer();
  if (!commandBuffer) {
    log::fatal("Invalid command buffer");
    throw std::runtime_error("Invalid command buffer");
  }

  try {
    commandBuffer.end();
  } catch (const vk::SystemError& err) {
    log::fatal("failed to record command buffer");
    throw std::runtime_error("failed to record command buffer");
  }

  vk::Result result =
      this->swapchain.submitCommandBuffers(&commandBuffer, &currentImageIndex);

  if (result == vk::Result::eErrorOutOfDateKHR ||
      result == vk::Result::eSuboptimalKHR) {
    // window size was changed, or swapchain out of date, recreate swapChain
    throw std::runtime_error("swapchain out of date");
  } else if (result != vk::Result::eSuccess) {
    throw std::runtime_error("failed to present swap chain image");
  }

  isFrameStarted = false;
  currentFrameIndex = (currentFrameIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(vk::CommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "Can't call beginSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't begin render pass on command buffer from a different frame");

  vk::RenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.renderPass = this->swapchain.getRenderPass();
  renderPassInfo.framebuffer =
      this->swapchain.getFrameBuffer(this->currentImageIndex);
  renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
  renderPassInfo.renderArea.extent = this->swapchain.getExtent();

  vk::ClearValue clearColor = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

  vk::Viewport viewport{0.0f,
                        0.0f,
                        static_cast<float>(this->swapchain.width()),
                        static_cast<float>(this->swapchain.height()),
                        0.0f,
                        1.0f};
  vk::Rect2D scissor{{0, 0}, this->swapchain.getExtent()};

  commandBuffer.setViewport(0, 1, &viewport);
  commandBuffer.setScissor(0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(vk::CommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "Can't call endSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "Can't end render pass on command buffer from a different frame");
  commandBuffer.endRenderPass();
}

void Renderer::createCommandBuffers() {
  this->commandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);

  vk::CommandBufferAllocateInfo allocInfo = {};
  allocInfo.level = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandPool = this->device.getCommandPool();
  allocInfo.commandBufferCount = static_cast<u32>(this->commandBuffers.size());

  try {
    this->commandBuffers =
        this->device.get()->allocateCommandBuffers(allocInfo);
    log::verbose("created all commandBuffers");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to allocate command buffers");
    throw std::runtime_error("failed to allocate command buffers");
  }
}

}  // namespace hep
