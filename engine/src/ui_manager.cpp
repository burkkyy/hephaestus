#include "ui_manager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace hep {

static void checkVkResult(VkResult result) {
  if (result == VK_SUCCESS) { return; }
  log::error("[vulkan] Error: vk::Result: ", result);
}

UIManager::Builder::Builder(Window& window,
                            Device& device,
                            Renderer& renderer,
                            const vk::DescriptorPool& descriptorPool) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

  ImGui_ImplVulkan_InitInfo initInfo = {};
  device.populateImGuiInitInfo(initInfo);
  initInfo.DescriptorPool = descriptorPool;
  renderer.populateImGuiInitInfo(initInfo);
  initInfo.CheckVkResultFn = checkVkResult;

  ImGui_ImplVulkan_Init(&initInfo);
}

UIManager::Builder& UIManager::Builder::lightTheme() {
  ImGui::StyleColorsLight();
  return *this;
}

UIManager::Builder& UIManager::Builder::darkTheme() {
  ImGui::StyleColorsDark();
  return *this;
}

UIManager::Builder& UIManager::Builder::setMultiViewport(bool enable) {
  ImGuiIO& io = ImGui::GetIO();
  if (enable) {
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  } else {
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
  }
  return *this;
}

UIManager::Builder& UIManager::Builder::setDocking(bool enable) {
  ImGuiIO& io = ImGui::GetIO();
  if (enable) {
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  } else {
    io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
  }
  return *this;
}

UIManager::Builder& UIManager::Builder::setKeyboard(bool enable) {
  ImGuiIO& io = ImGui::GetIO();
  if (enable) {
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  } else {
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
  }
  return *this;
}

std::unique_ptr<UIManager> UIManager::Builder::build() {
  return std::make_unique<UIManager>();
}

UIManager::UIManager() {
  /**
   * When viewports are enabled we tweak WindowRounding/WindowBg so
   * platform windows can look identical to regular ones.
   */
  ImGuiStyle& style = ImGui::GetStyle();
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }
}

UIManager::~UIManager() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UIManager::registerPanel(std::unique_ptr<Panel> panel) {
  this->panels.push_back(std::move(panel));
}

void UIManager::updatePanels() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                               ImGuiDockNodeFlags_PassthruCentralNode);

  for (auto& panel : panels) { panel->onUpdate(); }
}

void UIManager::renderPanels(vk::CommandBuffer commandBuffer) {
  ImGui::Render();
  ImDrawData* mainDrawData = ImGui::GetDrawData();

  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  ImGui_ImplVulkan_RenderDrawData(mainDrawData, commandBuffer);
}

}  // namespace hep
