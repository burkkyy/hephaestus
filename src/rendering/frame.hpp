#pragma once

#include <vulkan/vulkan.hpp>

#include "core/device.hpp"
#include "util/utils.hpp"

namespace hep {

/**
 * Class for handling a render target
 *
 * @note currently only supports 2D target frame
 */
class Frame {
 public:
  Frame(const Frame&) = delete;
  Frame& operator=(const Frame&) = delete;

  Frame(Device& device, vk::Extent2D extent, vk::Format format,
        vk::ImageUsageFlags usage, vk::RenderPass& renderPass);
  ~Frame();

 private:
  Device& device;
  vk::Extent2D extent;
  vk::Format format;
  vk::DeviceMemory memory;
  vk::Image image;
  vk::ImageView view;
  vk::Framebuffer framebuffer;
};

}  // namespace hep
