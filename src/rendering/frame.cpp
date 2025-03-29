#include "frame.hpp"

namespace hep {

Frame::Frame(Device& device, vk::Extent2D extent, vk::Format format,
             vk::ImageUsageFlags usage, vk::RenderPass& renderPass)
    : device{device}, extent{extent}, format{format} {
  // Step 1: allocate memory and create image
  vk::ImageCreateInfo imageInfo{};
  imageInfo.imageType = vk::ImageType::e2D;
  imageInfo.format = this->format;
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
                                   this->image, this->memory);

  // Step 2: create image views
  vk::ImageViewCreateInfo imageViewInfo{};
  imageViewInfo.image = this->image;
  imageViewInfo.viewType = vk::ImageViewType::e2D;
  imageViewInfo.format = this->format;
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
    this->view = this->device.get()->createImageView(imageViewInfo);
  } catch (const vk::SystemError& error) {
    log::fatal("failed to create vk::ImageView in Frame. Error: ",
               error.what());
    throw std::runtime_error("failed to create vk::ImageView");
  }

  // Step 3: create frame buffer
  vk::FramebufferCreateInfo framebufferInfo{};
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = &this->view;
  framebufferInfo.width = this->extent.width;
  framebufferInfo.height = this->extent.height;
  framebufferInfo.layers = 1;

  try {
    this->framebuffer = this->device.get()->createFramebuffer(framebufferInfo);
  } catch (const vk::SystemError& error) {
    log::trace("failed to create vk::Framebuffer in Frame. Error: ",
               error.what());
    throw std::runtime_error("failed to create vk::Framebuffer");
  }
}

Frame::~Frame() {
  this->device.get()->destroyFramebuffer(this->framebuffer);
  this->device.get()->destroyImageView(this->view);
  this->device.get()->destroyImage(this->image);
  this->device.get()->freeMemory(this->memory);
}

}  // namespace hep
