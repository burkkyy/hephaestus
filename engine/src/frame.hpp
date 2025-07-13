#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "device.hpp"

namespace hep {

/**
 * Class for handling a render target
 *
 * @note Considering using a builder for this class, to support
 * arbitrary number of attachments
 * @note should this class store its own render pass?
 * @note currently only supports 2D target frame
 */
class Frame {
 public:
  Frame(const Frame&) = delete;
  Frame& operator=(const Frame&) = delete;

  class Builder {
   public:
    Builder(Device& device) : device{device} {}

    Builder& setImageExtent(vk::Extent2D extent) {
      this->imageExtent = extent;
      return *this;
    }

    Builder& setImageFormat(vk::Format format) {
      this->imageFormat = format;
      return *this;
    }

    Builder& setImageUsage(vk::ImageUsageFlags usage) {
      this->imageUsage = usage;
      return *this;
    }

    std::unique_ptr<Frame> build() const;

   private:
    Device& device;
    vk::Extent2D imageExtent;
    vk::Format imageFormat;
    vk::ImageUsageFlags imageUsage;
    vk::RenderPass renderPass;
  };

  Frame(Device& device,
        vk::Extent2D extent,
        vk::Format format,
        vk::ImageUsageFlags usage,
        vk::RenderPass renderPass);
  ~Frame();

  vk::Format getImageFormat() { return this->imageFormat; }
  vk::ImageView getImageView() { return this->imageView; }
  vk::Format getDepthFormat() { return this->depthFormat; }
  vk::RenderPass getRenderPass() { return this->renderPass; }
  vk::Framebuffer getFramebuffer() { return this->framebuffer; }

  void resize(vk::Extent2D extent);

 private:
  vk::Format findDepthFormat();

  void createImageResources(vk::ImageUsageFlags usage);
  void createDepthResources(vk::ImageUsageFlags usage);
  void createRenderPass();
  void createFramebuffer();

  void destroyFramebuffer();
  void destoryRenderPass();
  void destroyDepthResources();
  void destroyImageResources();

  Device& device;
  vk::Extent2D extent;

  vk::ImageUsageFlags imageUsage;
  vk::Format imageFormat;
  vk::DeviceMemory imageMemory;
  vk::Image image;
  vk::ImageView imageView;

  vk::Format depthFormat;
  vk::DeviceMemory depthImageMemory;
  vk::Image depthImage;
  vk::ImageView depthImageView;

  vk::RenderPass renderPass;
  vk::Framebuffer framebuffer;
};

}  // namespace hep
