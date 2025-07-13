#pragma once

#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "types.hpp"

namespace alp {

struct FrameInfo {
  u32 frameIndex;
  double elapsedTime;
  double deltaTime;
  glm::vec2 currentFramebufferExtent;
};

}  // namespace alp
