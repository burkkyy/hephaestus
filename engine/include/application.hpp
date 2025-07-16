#include "device.hpp"
#include "event.hpp"
#include "frame_info.hpp"
#include "imgui_render_system.hpp"
#include "key_event.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "util/logger.hpp"
#include "window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vulkan/vulkan.hpp>

namespace alp {

struct ApplicationConfig {
  int width;
  int height;
  std::string name;
};

class Application {
 public:
  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  Application(const ApplicationConfig& config);

  ~Application();

  void run();

  void onEvent(KeyReleasedEvent& event);

  void registerScene(std::unique_ptr<Scene> scene);

 private:
  ApplicationConfig config;
  Window window;
  Device device;
  Renderer renderer;

  ImGuiRenderSystem imGuiRenderSystem;

  SceneManager sceneManager;

  bool isRunning = true;
};

}  // namespace alp
