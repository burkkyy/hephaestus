#include <imgui.h>

#include <alphane>
#include <iostream>
#include <stdexcept>

class TestbedScene : public alp::Scene {
 public:
  void onAttach() override {
    createQuad({0.0f, 0.0f}, .2, .2);
    createQuad({-1.0f, -1.0f}, .2, .2);
    createQuad({.0f, -1.0f}, .2, .2);
    createQuad({-1, 0}, .1, .1);
  }

  void onImGuiRender() override {
    ImGui::Begin("Testbed Scene");
    ImGui::Text("This is a window made from TestbedScene");
    ImGui::End();
  }
};

class DebugPanel : public alp::Panel {
 public:
  void onUpdate(const alp::FrameInfo& frameInfo) override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, this->bgColor);
    ImGui::Begin("debug");

    if (ImGui::TreeNode("debug window preferences")) {
      ImGui::ColorEdit3("debug background color", (float*)&this->bgColor);
      ImGui::TreePop();
    }

    ImGui::Text("ImGui average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Text("elapsedTime: %.2f", frameInfo.elapsedTime);

    ImGui::Text("averageDeltaTime over 100 samples: %.4f ms",
                this->averageDeltaTime * 1000.0f);

    ImGui::Text("averageFrameRate over 100 samples: %.f fps",
                this->averageFrameRate);

    if (averageDeltaTime != 0.0f) {
      averageFrameRate = 1.0f / averageDeltaTime;
    }

    if (deltaTimeSamplesCount >= 100) {
      if (deltaTimeSamplesCount != 0) {
        averageDeltaTime = deltaTimeSamples / deltaTimeSamplesCount;
      }
      deltaTimeSamples = 0.0f;
      deltaTimeSamplesCount = 0;
    } else {
      deltaTimeSamples += frameInfo.deltaTime;
      deltaTimeSamplesCount++;
    }

    ImGui::End();
    ImGui::PopStyleColor();
  }

 private:
  ImVec4 bgColor = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);

  float averageFrameRate = 0.0f;
  float averageDeltaTime = 0.01f;
  float deltaTimeSamples = 0.0f;
  int deltaTimeSamplesCount = 0;
};

class TestbedPanel : public alp::Panel {
 public:
  void onUpdate(const alp::FrameInfo& frameInfo) override {
    ImGui::Begin("Testbed");
    ImGui::Text("This is a testbed panel");
    ImGui::End();
  }
};

int main(int argc, const char** argv) {
  (void)argc;
  (void)argv;
  std::cout << __FILE__ << "::" << __LINE__ << '\n';

  alp::Application app{{.width = 750, .height = 1000, .name = "Hep"}};
  app.registerScene(std::make_unique<TestbedScene>());

  app.run();

  return EXIT_SUCCESS;
}
