#pragma once

#include <memory>

#include "device.hpp"
#include "model.hpp"
#include "pipeline.hpp"
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
  void createPipelineLayout();
  void createPipeline(vk::RenderPass renderPass);
  void render();

  void sierpinski(std::vector<Model::Vertex>& vertices, int depth,
                  glm::vec2 left, glm::vec2 right, glm::vec2 top);

  Window window;
  Device device;
  Renderer renderer;
  Pipeline pipeline;
  vk::PipelineLayout pipelineLayout;
  std::unique_ptr<Model> model;
};

}  // namespace hep
