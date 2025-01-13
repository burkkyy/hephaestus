#include "swapchain.hpp"

#include <algorithm>
#include <limits>

#include "util/logger.hpp"

namespace hep {

Swapchain::Swapchain(Device& device, vk::Extent2D extent)
    : device{device}, extent{extent} {
  setDefaultCreateInfo();
  createSwapchain();
  createImageViews();
  createRenderPass();
  createFramebuffers();
}

Swapchain::~Swapchain() {
  for (auto framebuffer : this->framebuffers) {
    this->device.get()->destroyFramebuffer(framebuffer);
    log::verbose("destroyed vk::Framebuffer");
  }

  this->device.get()->destroyRenderPass(this->renderPass);
  log::verbose("destroyed vk::RenderPass");

  for (auto imageView : imageViews) {
    this->device.get()->destroyImageView(imageView);
    log::verbose("destroyed vk::ImageView");
  }

  this->device.get()->destroySwapchainKHR(this->swapchain);
  log::verbose("destroyed vk::SwapchainKHR");
}

void Swapchain::setDefaultCreateInfo() {
  SwapchainSupportDetails swapchainSupport = this->device.getSwapchainSupport();

  vk::SurfaceFormatKHR surfaceFormat =
      chooseSurfaceFormat(swapchainSupport.formats);
  vk::PresentModeKHR presentMode =
      choosePresentMode(swapchainSupport.presentModes);
  this->extent = chooseExtent(swapchainSupport.capabilities);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
  if (swapchainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapchainSupport.capabilities.maxImageCount) {
    imageCount = swapchainSupport.capabilities.maxImageCount;
  }

  this->swapchainCreateInfo = {vk::SwapchainCreateFlagsKHR(),
                               this->device.getSurface(),
                               imageCount,
                               surfaceFormat.format,
                               surfaceFormat.colorSpace,
                               this->extent,
                               1,
                               vk::ImageUsageFlagBits::eColorAttachment};

  QueueFamilyIndices indices = this->device.getQueueIndices();
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    swapchainCreateInfo.queueFamilyIndexCount = 2;
    swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
  }

  swapchainCreateInfo.preTransform =
      swapchainSupport.capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  swapchainCreateInfo.presentMode = presentMode;
  swapchainCreateInfo.clipped = VK_TRUE;

  swapchainCreateInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

  this->imageFormat = surfaceFormat.format;
}

void Swapchain::createSwapchain() {
  try {
    this->swapchain =
        this->device.get()->createSwapchainKHR(this->swapchainCreateInfo);
    log::verbose("created vk::SwapchainKHR");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create swapchain");
    throw std::runtime_error("failed to create swapchain");
  }

  this->images = this->device.get()->getSwapchainImagesKHR(this->swapchain);
}

void Swapchain::createImageViews() {
  this->imageViews.resize(this->images.size());

  for (size_t i = 0; i < this->images.size(); i++) {
    vk::ImageViewCreateInfo createInfo = {};
    createInfo.image = this->images[i];
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = imageFormat;
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    try {
      this->imageViews[i] = this->device.get()->createImageView(createInfo);
    } catch (const vk::SystemError& err) {
      log::fatal("failed to create all vk::ImageView");
      throw std::runtime_error("failed to create all vk::ImageView");
    }
  }

  log::verbose("created all vk::ImageView");
}

void Swapchain::createRenderPass() {
  vk::AttachmentDescription colorAttachment = {};
  colorAttachment.format = this->imageFormat;
  colorAttachment.samples = vk::SampleCountFlagBits::e1;
  colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
  colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
  colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
  colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

  vk::SubpassDescription subpass = {};
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  vk::RenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  try {
    this->renderPass = this->device.get()->createRenderPass(renderPassInfo);
    log::verbose("created vk::RenderPass");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::RenderPass");
    throw std::runtime_error("failed to create vk::RenderPass");
  }
}

void Swapchain::createFramebuffers() {
  this->framebuffers.resize(this->imageViews.size());
  for (size_t i = 0; i < this->imageViews.size(); i++) {
    vk::ImageView attachments[] = {this->imageViews[i]};

    vk::FramebufferCreateInfo createInfo = {};
    createInfo.renderPass = this->renderPass;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = attachments;
    createInfo.width = this->extent.width;
    createInfo.height = this->extent.height;
    createInfo.layers = 1;
    try {
      this->framebuffers[i] = this->device.get()->createFramebuffer(createInfo);
      log::verbose("created vk::Frambuffer");
    } catch (const vk::SystemError& err) {
      log::verbose("failed to create vk::Framebuffer");
      throw std::runtime_error("failed to create vk::Framebuffer");
    }
  }
}

vk::SurfaceFormatKHR Swapchain::chooseSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
  if (availableFormats.size() == 1 &&
      availableFormats[0].format == vk::Format::eUndefined) {
    return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
  }

  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
        availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return availableFormat;
    }
  }

  log::info("No available surface formats, choosing default.");
  return availableFormats[0];
}

vk::PresentModeKHR Swapchain::choosePresentMode(
    const std::vector<vk::PresentModeKHR> availablePresentModes) {
  vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
      return availablePresentMode;
    } else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
      bestMode = availablePresentMode;
    }
  }

  log::warning("Present mode MAILBOX not available.");
  return bestMode;
}

vk::Extent2D Swapchain::chooseExtent(
    const vk::SurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<u32>::max()) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D choosenExtent = {};
    choosenExtent.width =
        std::clamp(this->extent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    choosenExtent.height =
        std::clamp(this->extent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);
    return choosenExtent;
  }
}

}  // namespace hep
