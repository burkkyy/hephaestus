#pragma once

#include "ui/ui_component.hpp"

namespace hep {

class ImGuiDemoWindow : public UIComponent {
 public:
  void draw(const FrameInfo& frameInfo) override {
    ImGui::ShowDemoWindow(&this->show);
  }
};

}  // namespace hep
