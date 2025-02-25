#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
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

 private:
  Window window;
  Device device;
  Renderer renderer;
};

}  // namespace hep
