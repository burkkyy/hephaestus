#include "swapchain.hpp"

#include <algorithm>
#include <limits>

#include "util/logger.hpp"

namespace hep {

Swapchain::Swapchain(Device& device, vk::Extent2D extent)
    : device{device}, extent{extent} {
  setDefaultCreateInfo();
  create();
}

Swapchain::~Swapchain() {
  this->device.get()->destroySwapchainKHR(this->swapchain);
  log::verbose("Destroyed vk::SwapchainKHR.");
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

  this->createInfo = {vk::SwapchainCreateFlagsKHR(),
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
    createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = vk::SharingMode::eExclusive;
  }

  createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

  this->imageFormat = surfaceFormat.format;
}

void Swapchain::create() {
  try {
    this->swapchain = device.get()->createSwapchainKHR(createInfo);
    log::verbose("created vk::SwapchainKHR");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create swapchain");
    throw std::runtime_error("failed to create swapchain");
  }

  this->images = device.get()->getSwapchainImagesKHR(this->swapchain);
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
