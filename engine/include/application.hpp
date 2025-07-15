#include "descriptors/descriptor_pool.hpp"
#include "device.hpp"
#include "event.hpp"
#include "frame_info.hpp"
#include "key_event.hpp"
#include "render_systems/quad_render_system.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "ui_manager.hpp"
#include "util/logger.hpp"
#include "window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

  void registerPanel(std::unique_ptr<Panel> panel);
  void addQuad(glm::vec2 position, float width, float height);

  void onEvent(KeyReleasedEvent& event);

 private:
  ApplicationConfig config;
  Window window;
  Device device;
  Renderer renderer;

  std::unique_ptr<alp::DescriptorPool> imguiDescriptorPool;
  std::unique_ptr<UIManager> uiManager;

  QuadRenderSystem quadRenderSystem;
  Scene scene;

  bool isRunning = true;
};

}  // namespace alp
