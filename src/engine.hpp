#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "events/event.hpp"
#include "events/key_event.hpp"
#include "renderer.hpp"
#include "window.hpp"

namespace hep {

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

class Engine {
 public:
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  Engine();
  ~Engine();

  void run();

  void onEvent(KeyReleasedEvent& event);

 private:
  Window window;
  Device device;
  Renderer renderer;

  bool isRunning = true;
};

}  // namespace hep
