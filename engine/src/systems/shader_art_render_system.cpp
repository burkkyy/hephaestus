#include "shader_art_render_system.hpp"

#include "util/logger.hpp"

namespace alp {

ShaderArtRenderSystem::ShaderArtRenderSystem(Device& device,
                                             vk::Extent2D extent)
    : device{device}, extent{extent}, pipeline{device} {
  this->frame = Frame::Builder(device)
                    .setImageExtent(extent)
                    .setImageFormat(vk::Format::eR8G8B8A8Unorm)
                    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                                   vk::ImageUsageFlagBits::eSampled)
                    .build();

  createPipelineLayout();
  createPipeline();
  createSampler();

  createImGuiTexture();
  //  createDescriptorResources();

  Model::Builder quadBuilder{};
  quadBuilder.vertices = {{{-1.0f, -1.0f}, {1, 0, 0}},
                          {{-1.0f, 1.0f}, {1, 0, 0}},
                          {{1.0f, 1.0f}, {0, 1, 0}},
                          {{1.0f, -1.0f}, {0, 0, 1}}};
  quadBuilder.indicies = {0, 1, 2, 0, 2, 3};
  this->quad = std::make_unique<Model>(this->device, quadBuilder);

  pushConstant.transform = glm::mat4(1.0f);

  // EventSystem::get().addListener<ShaderArtWindowResizeEvent>(
  //     std::bind(&ShaderArtRenderSystem::onWindowResizeEvent, this,
  //               std::placeholders::_1));
}

ShaderArtRenderSystem ::~ShaderArtRenderSystem() {
  ImGui_ImplVulkan_RemoveTexture(this->imguiDescriptorSet);
  log::trace("destroyed imgui texture");

  this->device.get()->destroySampler(sampler);
  log::trace("destroyed vk::Sampler");

  this->device.get()->destroyPipelineLayout(this->pipelineLayout);
  log::trace("destroyed vk::PipelineLayout");
}

void ShaderArtRenderSystem::render(vk::CommandBuffer commandBuffer,
                                   FrameInfo frameInfo) {
  // begin render pass
  vk::RenderPassBeginInfo beginInfo{};
  beginInfo.renderPass = this->frame->getRenderPass();
  beginInfo.framebuffer = this->frame->getFramebuffer();
  beginInfo.renderArea.offset = vk::Offset2D{0, 0};
  beginInfo.renderArea.extent = this->extent;

  std::array<vk::ClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};
  beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  beginInfo.pClearValues = clearValues.data();

  commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

  // Set graphics pipeline dynamic state
  vk::Viewport viewport{0.0f,
                        0.0f,
                        static_cast<float>(this->extent.width),
                        static_cast<float>(this->extent.height),
                        0.0f,
                        1.0f};
  vk::Rect2D scissor{{0, 0}, this->extent};

  commandBuffer.setViewport(0, 1, &viewport);
  commandBuffer.setScissor(0, 1, &scissor);

  // record to command buffer
  this->pipeline.bind(commandBuffer);

  pushConstant.data = {this->extent.width, this->extent.height,
                       frameInfo.elapsedTime, 0.0f};

  quad->bind(commandBuffer);

  pushConstant.color = {1.0f, 0.0f, 0.0f, 1.0f};
  commandBuffer.pushConstants(
      this->pipelineLayout,
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
      sizeof(PushConstantData), &pushConstant);

  quad->draw(commandBuffer);

  // end render pass
  commandBuffer.endRenderPass();
}

// vk::DescriptorSet ShaderArtRenderSystem::getImageDescriptorSet() {
//   vk::DescriptorImageInfo imageInfo{};
//   imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
//   imageInfo.imageView = this->frame->getImageView();
//   imageInfo.sampler = this->sampler;

//   vk::DescriptorSet descriptorSet;
//   DescriptorWriter(*this->imageDescriptorSetLayout,
//   *this->imageDescriptorPool)
//       .writeImage(0, &imageInfo)
//       .build(descriptorSet);

//   return descriptorSet;
// }

ImTextureID ShaderArtRenderSystem::getImageTextureID() {
  return (ImTextureID)this->imguiDescriptorSet;
}

// void ShaderArtRenderSystem::onWindowResizeEvent(
//     ShaderArtWindowResizeEvent& event) {
//   this->extent = event.getExtent();
//   // log::info("Window resize event callback: ", this->extent.width, "x",
//   //           this->extent.height);
//   // log::info("Recreating shader art viewport frame...");

//   this->device.get()->waitIdle();  // NOTE: This is super slow

//   ImGui_ImplVulkan_RemoveTexture(this->imguiDescriptorSet);
//   // log::trace("destroyed imgui texture");

//   this->frame->resize(this->extent);

//   createImGuiTexture();
// }

void ShaderArtRenderSystem::createPipelineLayout() {
  vk::PushConstantRange pushConstantRange{};

  pushConstantRange.stageFlags =
      vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(PushConstantData);

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  try {
    this->pipelineLayout =
        this->device.get()->createPipelineLayout(pipelineLayoutInfo);
    log::trace("created vk::PipelineLayout");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::PipelineLayout");
    throw std::runtime_error("failed to create vk::PipelineLayout");
  }
}

void ShaderArtRenderSystem::createPipeline() {
  assert(this->pipelineLayout != nullptr &&
         "Cannot create pipeline before pipeline layout");

  assert(this->frame != nullptr &&
         "Cannot create pipeline with no render pass");

  this->pipeline.create("shaders/quad.vert.spv", "shaders/art.frag.spv",
                        this->pipelineLayout, this->frame->getRenderPass());
}

void ShaderArtRenderSystem::createSampler() {
  vk::SamplerCreateInfo samplerInfo{};
  samplerInfo.magFilter = vk::Filter::eLinear;
  samplerInfo.minFilter = vk::Filter::eLinear;
  samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
  samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
  samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
  samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
  samplerInfo.unnormalizedCoordinates = vk::False;
  samplerInfo.compareEnable = vk::True;
  samplerInfo.compareOp = vk::CompareOp::eAlways;
  samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
  samplerInfo.anisotropyEnable = vk::False;

  try {
    this->sampler = this->device.get()->createSampler(samplerInfo);
  } catch (const vk::SystemError& error) {
    log::fatal("failed to create sampler. Error: ", error.what());
    throw std::runtime_error("failed to create sampler");
  }
}

// void ShaderArtRenderSystem::createDescriptorResources() {
//   this->imageDescriptorPool =
//       DescriptorPool::Builder(this->device)
//           .addPoolSize(vk::DescriptorType::eCombinedImageSampler, 1)
//           .setMaxSets(1)
//           .build();

//   this->imageDescriptorSetLayout =
//       DescriptorSetLayout::Builder(this->device)
//           .addBinding(0, vk::DescriptorType::eCombinedImageSampler,
//                       vk::ShaderStageFlagBits::eFragment)
//           .build();

//   vk::DescriptorImageInfo imageInfo{};
//   imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
//   imageInfo.imageView = this->frame->getImageView();
//   imageInfo.sampler = sampler;

//   vk::DescriptorSet imguiImageDescriptorSet;
//   DescriptorWriter(*imageDescriptorSetLayout, *imageDescriptorPool)
//       .writeImage(0, &imageInfo)
//       .build(imguiImageDescriptorSet);
// }

void ShaderArtRenderSystem::createImGuiTexture() {
  assert(this->sampler != nullptr);
  assert(this->frame != nullptr);

  this->imguiDescriptorSet =
      ImGui_ImplVulkan_AddTexture(this->sampler, this->frame->getImageView(),
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  if (imguiDescriptorSet == 0) {
    log::error("ImGui_ImplVulkan_AddTexture failed!");
  }
  // log::trace("created imgui texture");
}

}  // namespace alp
