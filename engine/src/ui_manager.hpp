#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "frame_info.hpp"
#include "panel.hpp"
#include "renderer.hpp"
#include "window.hpp"

namespace alp {

class UIManager {
 public:
  UIManager(const UIManager&) = delete;
  UIManager& operator=(const UIManager&) = delete;

  class Builder {
   public:
    Builder(Window& window,
            Device& device,
            Renderer& renderer,
            const vk::DescriptorPool& descriptorPool);

    Builder& lightTheme();
    Builder& darkTheme();

    Builder& setMultiViewport(bool enable);
    Builder& setDocking(bool enable);
    Builder& setKeyboard(bool enable);

    std::unique_ptr<UIManager> build();
  };

  UIManager();
  ~UIManager();

  void registerPanel(std::unique_ptr<Panel> panel);

  void updatePanels(const FrameInfo& frameInfo);
  void renderPanels(vk::CommandBuffer commandBuffer);

 private:
  std::vector<std::unique_ptr<Panel>> panels;
};

}  // namespace alp
