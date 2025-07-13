#include "frame.hpp"

namespace alp {

std::unique_ptr<Frame> Frame::Builder::build() const {
  return std::make_unique<Frame>(this->device, this->imageExtent,
                                 this->imageFormat, this->imageUsage,
                                 this->renderPass);
}

Frame::Frame(Device& device,
             vk::Extent2D extent,
             vk::Format format,
             vk::ImageUsageFlags usage,
             vk::RenderPass renderPass)
    : device{device},
      extent{extent},
      imageFormat{format},
      imageUsage{usage},
      renderPass{renderPass} {
  createImageResources(usage);
  createDepthResources(vk::ImageUsageFlagBits::eDepthStencilAttachment);
  createRenderPass();
  createFramebuffer();
}

Frame::~Frame() {
  destroyFramebuffer();
  destoryRenderPass();
  destroyDepthResources();
  destroyImageResources();
}

void Frame::resize(vk::Extent2D extent) {
  this->extent = extent;
  this->device.get()->waitIdle();

  destroyFramebuffer();
  destroyDepthResources();
  destroyImageResources();

  createImageResources(this->imageUsage);
  createDepthResources(vk::ImageUsageFlagBits::eDepthStencilAttachment);
  createFramebuffer();
}

vk::Format Frame::findDepthFormat() {
  return this->device.findSupportedFormat(
      {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint,
       vk::Format::eD24UnormS8Uint},
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

void Frame::createImageResources(vk::ImageUsageFlags usage) {
  vk::ImageCreateInfo imageInfo{};
  imageInfo.imageType = vk::ImageType::e2D;
  imageInfo.format = this->imageFormat;
  imageInfo.extent.width = this->extent.width;
  imageInfo.extent.height = this->extent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = vk::SampleCountFlagBits::e1;
  imageInfo.tiling = vk::ImageTiling::eOptimal;
  imageInfo.usage = usage;
  imageInfo.sharingMode = vk::SharingMode::eExclusive;
  imageInfo.initialLayout = vk::ImageLayout::eUndefined;

  this->device.createImageWithInfo(imageInfo,
                                   vk::MemoryPropertyFlagBits::eDeviceLocal,
                                   this->image, this->imageMemory);

  vk::ImageViewCreateInfo imageViewInfo{};
  imageViewInfo.image = this->image;
  imageViewInfo.viewType = vk::ImageViewType::e2D;
  imageViewInfo.format = this->imageFormat;
  imageViewInfo.components.r = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.components.g = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.components.b = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.components.a = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  imageViewInfo.subresourceRange.baseMipLevel = 0;
  imageViewInfo.subresourceRange.levelCount = 1;
  imageViewInfo.subresourceRange.baseArrayLayer = 0;
  imageViewInfo.subresourceRange.layerCount = 1;

  try {
    this->imageView = this->device.get()->createImageView(imageViewInfo);
  } catch (const vk::SystemError& error) {
    log::fatal("failed to create image view in Frame. Error: ", error.what());
    throw std::runtime_error("failed to create vk::ImageView");
  }
}

void Frame::createDepthResources(vk::ImageUsageFlags usage) {
  this->depthFormat = findDepthFormat();

  vk::ImageCreateInfo imageInfo{};
  imageInfo.imageType = vk::ImageType::e2D;
  imageInfo.format = this->depthFormat;
  imageInfo.extent.width = this->extent.width;
  imageInfo.extent.height = this->extent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = vk::SampleCountFlagBits::e1;
  imageInfo.tiling = vk::ImageTiling::eOptimal;
  imageInfo.usage = usage;
  imageInfo.sharingMode = vk::SharingMode::eExclusive;
  imageInfo.initialLayout = vk::ImageLayout::eUndefined;

  this->device.createImageWithInfo(imageInfo,
                                   vk::MemoryPropertyFlagBits::eDeviceLocal,
                                   this->depthImage, this->depthImageMemory);

  vk::ImageViewCreateInfo imageViewInfo{};
  imageViewInfo.image = this->depthImage;
  imageViewInfo.viewType = vk::ImageViewType::e2D;
  imageViewInfo.format = this->depthFormat;
  imageViewInfo.components.r = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.components.g = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.components.b = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.components.a = vk::ComponentSwizzle::eIdentity;
  imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
  imageViewInfo.subresourceRange.baseMipLevel = 0;
  imageViewInfo.subresourceRange.levelCount = 1;
  imageViewInfo.subresourceRange.baseArrayLayer = 0;
  imageViewInfo.subresourceRange.layerCount = 1;

  try {
    this->depthImageView = this->device.get()->createImageView(imageViewInfo);
  } catch (const vk::SystemError& error) {
    log::fatal("failed to create depth image view in Frame. Error: ",
               error.what());
    throw std::runtime_error("failed to create vk::ImageView");
  }
}

void Frame::createRenderPass() {
  vk::AttachmentDescription depthAttachment{};
  depthAttachment.format = this->depthFormat;
  depthAttachment.samples = vk::SampleCountFlagBits::e1;
  depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
  depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
  depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
  depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

  vk::AttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

  vk::AttachmentDescription colorAttachment = {};
  colorAttachment.format = this->imageFormat;
  colorAttachment.samples = vk::SampleCountFlagBits::e1;
  colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
  colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
  colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
  colorAttachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

  vk::AttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

  vk::SubpassDescription subpass = {};
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  vk::SubpassDependency dependency = {};
  dependency.dstSubpass = 0;
  dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
                             vk::AccessFlagBits::eDepthStencilAttachmentWrite;
  dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                            vk::PipelineStageFlagBits::eEarlyFragmentTests;
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                            vk::PipelineStageFlagBits::eEarlyFragmentTests;

  std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment,
                                                          depthAttachment};

  vk::RenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  try {
    this->renderPass = this->device.get()->createRenderPass(renderPassInfo);
    log::trace("created vk::RenderPass for frame");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::RenderPass");
    throw std::runtime_error("failed to create vk::RenderPass");
  }
}

void Frame::createFramebuffer() {
  std::array<vk::ImageView, 2> attachments = {this->imageView,
                                              this->depthImageView};

  vk::FramebufferCreateInfo framebufferInfo{};
  framebufferInfo.renderPass = this->renderPass;
  framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = this->extent.width;
  framebufferInfo.height = this->extent.height;
  framebufferInfo.layers = 1;

  try {
    this->framebuffer = this->device.get()->createFramebuffer(framebufferInfo);
  } catch (const vk::SystemError& error) {
    log::error("failed to create vk::Framebuffer in Frame. Error: ",
               error.what());
    throw std::runtime_error("failed to create vk::Framebuffer");
  }
}

void Frame::destroyFramebuffer() {
  this->device.get()->destroyFramebuffer(this->framebuffer);
  // log::trace("destroyed frame framebuffer resources");
}

void Frame::destoryRenderPass() {
  this->device.get()->destroyRenderPass(this->renderPass);
  // log::trace("destroyed frame vk::RenderPass");
}

void Frame::destroyDepthResources() {
  this->device.get()->destroyImageView(this->depthImageView);
  this->device.get()->destroyImage(this->depthImage);
  this->device.get()->freeMemory(this->depthImageMemory);
  // log::trace("destroyed frame depth resources");
}

void Frame::destroyImageResources() {
  this->device.get()->destroyImageView(this->imageView);
  this->device.get()->destroyImage(this->image);
  this->device.get()->freeMemory(this->imageMemory);
  // log::trace("destroyed frame image resources");
}

}  // namespace alp
