#pragma once

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "util/types.hpp"

namespace hep {

struct FrameInfo {
  vk::CommandBuffer commandBuffer;
  u32 frameIndex;
  double elapsedTime;
  double deltaTime;
  glm::vec2 currentFramebufferExtent;
};

}  // namespace hep
