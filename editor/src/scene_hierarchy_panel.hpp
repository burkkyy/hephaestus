#include <imgui.h>

#include <alphane>

class SceneHierarchyPanel : public alp::Panel {
 public:
  void onUpdate(const alp::FrameInfo& frameInfo) override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->bgColor);
    ImGui::Begin("Scene Hierarchy");

    if (ImGui::TreeNode("debug window preferences")) {
      ImGui::ColorEdit3("debug background color", (float*)&this->bgColor);
      ImGui::TreePop();
    }

    ImGui::End();
    ImGui::PopStyleColor();
  }

 private:
  ImVec4 bgColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
};
