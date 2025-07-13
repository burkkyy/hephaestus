#include "descriptors/descriptor_pool.hpp"
#include "device.hpp"
#include "event.hpp"
#include "frame_info.hpp"
#include "key_event.hpp"
#include "renderer.hpp"
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

namespace hep {

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

  void onEvent(KeyReleasedEvent& event);

 private:
  ApplicationConfig config;
  Window window;
  Device device;
  Renderer renderer;

  std::unique_ptr<hep::DescriptorPool> imguiDescriptorPool;
  std::unique_ptr<UIManager> uiManager;

  bool isRunning = true;
};

}  // namespace hep
