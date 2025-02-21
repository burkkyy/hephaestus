#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "renderer.hpp"
#include "window.hpp"

namespace hep {

#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 200

class Engine {
 public:
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  Engine();
  ~Engine();

  void run();

 private:
  void createPipelineLayout();
  void createPipeline(vk::RenderPass renderPass);
  void render();

  Window window;
  Device device;
  Renderer renderer;
  Pipeline pipeline;
  vk::PipelineLayout pipelineLayout;
};

}  // namespace hep
