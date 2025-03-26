#pragma once

#include <imgui.h>

#include <memory>
#include <vector>

#include "device.hpp"
#include "frame_info.hpp"
#include "renderer.hpp"
#include "ui_component.hpp"
#include "window.hpp"

namespace hep {

class UISystem {
 public:
  UISystem(Window& window, Device& device, Renderer& renderer,
           const vk::DescriptorPool& descriptorPool);
  ~UISystem();

  void setup();
  void update(const FrameInfo& frameInfo);
  void render(vk::CommandBuffer commandBuffer);

  void addComponent(std::unique_ptr<UIComponent> component);

 private:
  bool showDemoWindow = true;

  std::vector<std::unique_ptr<UIComponent>> components;
};

}  // namespace hep
