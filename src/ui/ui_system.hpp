#pragma once

#include <imgui.h>

#include <memory>
#include <vector>

#include "rendering/core/device.hpp"
#include "rendering/core/window.hpp"
#include "rendering/frame_info.hpp"
#include "rendering/renderer.hpp"
#include "ui_component.hpp"

namespace hep {

class UISystem {
 public:
  UISystem(const UISystem&) = delete;
  UISystem& operator=(const UISystem&) = delete;

  class Builder {
   public:
    Builder(Window& window, Device& device, Renderer& renderer,
            const vk::DescriptorPool& descriptorPool);

    Builder& lightTheme();
    Builder& darkTheme();
    // Builder& setTheme();
    Builder& setMultiViewport(bool enable);
    Builder& setDocking(bool enable);
    Builder& setKeyboard(bool enable);
    Builder& addComponent(std::unique_ptr<UIComponent> component);
    // Builder& addFont(); _TODO_
    Builder& showDemoWindow();

    std::unique_ptr<UISystem> build();

   private:
    std::vector<std::unique_ptr<UIComponent>> components;
  };

  UISystem(std::vector<std::unique_ptr<UIComponent>> components);
  ~UISystem();

  void setup();
  void update(const FrameInfo& frameInfo);
  void render(vk::CommandBuffer commandBuffer);

 private:
  bool showDemoWindow = true;

  std::vector<std::unique_ptr<UIComponent>> components;
};

}  // namespace hep
