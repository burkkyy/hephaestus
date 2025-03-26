#include "ui_system.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "components/imgui_demo_window.hpp"
#include "util/logger.hpp"

namespace hep {

static void checkVulkanResult(VkResult error) {
  if (error == VK_SUCCESS) { return; }
  log::error("[vulkan] Error: vk::Result: ", error);
}

UISystem::Builder::Builder(Window& window, Device& device, Renderer& renderer,
                           const vk::DescriptorPool& descriptorPool) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

  ImGui_ImplVulkan_InitInfo initInfo = {};
  device.populateImGuiInitInfo(initInfo);
  initInfo.DescriptorPool = descriptorPool;
  renderer.populateImGuiInitInfo(initInfo);
  initInfo.CheckVkResultFn = checkVulkanResult;

  ImGui_ImplVulkan_Init(&initInfo);
}

UISystem::Builder& UISystem::Builder::lightTheme() {
  ImGui::StyleColorsLight();
  return *this;
}

UISystem::Builder& UISystem::Builder::darkTheme() {
  ImGui::StyleColorsDark();
  return *this;
}

UISystem::Builder& UISystem::Builder::setMultiViewport(bool enable) {
  ImGuiIO& io = ImGui::GetIO();
  if (enable) {
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  } else {
    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
  }
  return *this;
}

UISystem::Builder& UISystem::Builder::setDocking(bool enable) {
  ImGuiIO& io = ImGui::GetIO();
  if (enable) {
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  } else {
    io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
  }
  return *this;
}

UISystem::Builder& UISystem::Builder::setKeyboard(bool enable) {
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

UISystem::Builder& UISystem::Builder::addComponent(
    std::unique_ptr<UIComponent> component) {
  this->components.push_back(std::move(component));
  return *this;
}

UISystem::Builder& UISystem::Builder::showDemoWindow() {
  this->components.push_back(std::make_unique<ImGuiDemoWindow>());
  return *this;
}

std::unique_ptr<UISystem> UISystem::Builder::build() {
  return std::make_unique<UISystem>(std::move(this->components));
}

UISystem::UISystem(std::vector<std::unique_ptr<UIComponent>> components)
    : components{std::move(components)} {
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

UISystem::~UISystem() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UISystem::setup() {
  for (auto& component : this->components) { component->setup(); }
}

void UISystem::update(const FrameInfo& frameInfo) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                               ImGuiDockNodeFlags_PassthruCentralNode);

  for (auto& component : this->components) {
    if (component->shouldDraw()) { component->draw(frameInfo); }
  }
}

void UISystem::render(vk::CommandBuffer commandBuffer) {
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
