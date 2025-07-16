#include "imgui_render_system.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace alp {

static void checkVkResult(VkResult result) {
  if (result == VK_SUCCESS) { return; }
  log::error("[vulkan] Error: vk::Result: ", result);
}

ImGuiRenderSystem::ImGuiRenderSystem(Window& window,
                                     Device& device,
                                     Renderer& renderer) {
  // 1. Construct descriptor pool
  this->descriptorPool =
      DescriptorPool::Builder(device)
          .addPoolSize(vk::DescriptorType::eCombinedImageSampler,
                       IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 5)
          .setPoolFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
          .setMaxSets(IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE + 5)
          .build();

  // 2. Construct ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

  ImGui_ImplVulkan_InitInfo initInfo = {};
  device.populateImGuiInitInfo(initInfo);
  initInfo.DescriptorPool = this->descriptorPool->get();
  renderer.populateImGuiInitInfo(initInfo);
  initInfo.CheckVkResultFn = checkVkResult;

  ImGui_ImplVulkan_Init(&initInfo);

  // 3. Enable dark theme
  ImGui::StyleColorsDark();

  // 4. Enable docking / keyboard
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

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

ImGuiRenderSystem::~ImGuiRenderSystem() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiRenderSystem::beginFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                               ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImGuiRenderSystem::endFrame(vk::CommandBuffer commandBuffer) {
  ImGui::Render();
  ImDrawData* mainDrawData = ImGui::GetDrawData();

  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  ImGui_ImplVulkan_RenderDrawData(mainDrawData, commandBuffer);
}

}  // namespace alp
