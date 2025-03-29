#pragma once

#include <imgui.h>

#include "rendering/frame_info.hpp"

namespace hep {

class UIComponent {
 public:
  UIComponent() = default;

  virtual void setup() {}
  virtual void draw(const FrameInfo& frameInfo) = 0;

  void display() { this->show = true; }
  void close() { this->show = false; }
  bool shouldDraw() { return this->show; }

 protected:
  bool show = true;
};

}  // namespace hep
