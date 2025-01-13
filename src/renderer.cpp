#include "renderer.hpp"

#include "util/logger.hpp"

namespace hep {

Renderer::Renderer(Window& window, Device& device)
    : window{window},
      device{device},
      swapchain{device, window.getExtent()},
      pipeline{device} {
  this->pipeline.create("shaders/triangle.vert.spv",
                        "shaders/triangle.frag.spv",
                        this->swapchain.getRenderPass());
  createCommandBuffers();
}

Renderer::~Renderer() { freeCommandBuffers(); }

void Renderer::createCommandBuffers() {
  this->commandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);

  vk::CommandBufferAllocateInfo allocInfo = {};
  allocInfo.commandPool = this->device.getCommandPool();
  allocInfo.level = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandBufferCount = static_cast<u32>(this->commandBuffers.size());

  try {
    this->commandBuffers =
        this->device.get()->allocateCommandBuffers(allocInfo);
  } catch (const vk::SystemError& err) {
    log::fatal("failed to allocate command buffers");
    throw std::runtime_error("failed to allocate command buffers");
  }

  for (size_t i = 0; i < this->commandBuffers.size(); i++) {
    vk::CommandBufferBeginInfo beginInfo = {};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

    try {
      this->commandBuffers[i].begin(beginInfo);
    } catch (const vk::SystemError& err) {
      log::fatal("failed to begin recording command buffer");
      throw std::runtime_error("failed to begin recording command buffer");
    }

    vk::RenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.renderPass = this->swapchain.getRenderPass();
    renderPassInfo.framebuffer = this->swapchain.getFrameBuffer(i);
    renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
    renderPassInfo.renderArea.extent = this->swapchain.getExtent();

    vk::ClearValue clearColor = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    this->commandBuffers[i].beginRenderPass(renderPassInfo,
                                            vk::SubpassContents::eInline);

    vk::Viewport viewport{
        0.0,  0.0, this->swapchain.width(), this->swapchain.height(),
        0.0f, 1.0f};
    vk::Rect2D scissor{{0, 0}, this->swapchain.getExtent()};

    this->commandBuffers.at(i).setViewport(0, 1, &viewport);
    this->commandBuffers.at(i).setScissor(0, 1, &scissor);

    // vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    // vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    this->pipeline.bind(this->commandBuffers.at(i));

    this->commandBuffers[i].draw(3, 1, 0, 0);

    this->commandBuffers[i].endRenderPass();

    try {
      this->commandBuffers[i].end();
    } catch (const vk::SystemError& err) {
      log::fatal("failed to record command buffer");
      throw std::runtime_error("failed to record command buffer");
    }
  }
}

void Renderer::freeCommandBuffers() {}

}  // namespace hep
