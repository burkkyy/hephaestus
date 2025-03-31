#pragma once

#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include "event.hpp"
#include "util/logger.hpp"

namespace hep {

class ShaderArtWindowResizeEvent : public Event {
 public:
  ShaderArtWindowResizeEvent(vk::Extent2D extent) : extent{extent} {}

  vk::Extent2D getExtent() { return this->extent; }

  const char* getName() const override { return "ShaderArtWindowResizeEvent"; }

 private:
  vk::Extent2D extent;
};

class ShaderArtWindow {
 public:
  void setup(ImTextureID textureId) { this->textureId = textureId; }
  void updateTextureId(ImTextureID textureId) { this->textureId = textureId; }

  void draw(const FrameInfo& frameInfo) {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->bgColor);
    ImGui::Begin(
        "Viewport", nullptr,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (this->textureId == 0) {
      log::error("textureId is null");
    } else {
      ImGui::Image(this->textureId, this->textureSize);
    }

    ImVec2 currentWindowSize = ImGui::GetWindowSize();

    if (this->previousWindowSize.x != currentWindowSize.x ||
        this->previousWindowSize.y != currentWindowSize.y) {
      ImVec2 windowContentRegionMin = ImGui::GetWindowContentRegionMin();
      ImVec2 windowContentRegionMax = ImGui::GetWindowContentRegionMax();
      float contentWidth = windowContentRegionMax.x - windowContentRegionMin.x;
      float contentHeight = windowContentRegionMax.y - windowContentRegionMin.y;

      this->textureSize = ImVec2(contentWidth, contentHeight);

      this->previousWindowSize = currentWindowSize;
      this->resized = true;
    }

    ImGui::End();
    ImGui::PopStyleColor();
  }

  void postDraw() {
    if (this->resized) {
      vk::Extent2D extent{static_cast<u32>(this->textureSize.x),
                          static_cast<u32>(this->textureSize.y)};

      /**
       * This is kinda of a hack, but for now this does the job of preventing
       * divide by zero
       */
      if (this->textureSize.x < 0.1f || this->textureSize.y < 0.1f) {
        extent.setHeight(1);
        extent.setWidth(1);
      }

      ShaderArtWindowResizeEvent event{extent};
      EventSystem::get().dispatch(event);

      this->resized = false;
    }
  }

  void toggleDisplay() { this->show = !this->show; }

 private:
  bool show = true;
  ImVec4 bgColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);

  ImVec2 previousWindowSize;
  bool resized = false;

  ImVec2 textureSize;
  ImTextureID textureId;
};

}  // namespace hep
