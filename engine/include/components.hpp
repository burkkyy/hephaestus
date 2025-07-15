#pragma once

#include "types.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace alp {

struct Transform2D {
  glm::vec2 translation;
  float rotation;

  glm::mat2 mat2() { return glm::mat2{1.0f}; }
};

struct Quad2D {
  float width;
  float height;
  glm::vec4 color;
};

}  // namespace alp
