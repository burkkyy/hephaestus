#include "ui_system.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "util/logger.hpp"

// UI componenets
#include "components/debug_overlay.hpp"
#include "components/imgui_demo_window.hpp"

namespace hep {

static void checkVulkanResult(VkResult error) {
  if (error == VK_SUCCESS) { return; }
  log::error("[vulkan] Error: vk::Result: ", error);
}

UISystem::UISystem(Window& window, Device& device, Renderer& renderer,
                   vk::DescriptorPool& descriptorPool) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;             // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
  // io.ConfigFlags |=
  //   ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform
  //  Windows
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

  ImGui_ImplVulkan_InitInfo initInfo = {};
  device.populateImGuiInitInfo(initInfo);
  initInfo.DescriptorPool = descriptorPool;
  renderer.populateImGuiInitInfo(initInfo);
  initInfo.CheckVkResultFn = checkVulkanResult;

  ImGui_ImplVulkan_Init(&initInfo);
}

UISystem::~UISystem() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UISystem::setup() {
  addComponent(std::make_unique<DebugOverlay>());
  // addComponent(std::make_unique<ImGuiDemoWindow>());

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

void UISystem::addComponent(std::unique_ptr<UIComponent> component) {
  this->components.push_back(std::move(component));
}

}  // namespace hep
