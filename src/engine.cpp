#include "engine.hpp"

namespace hep {

Engine::Engine()
    : window{WINDOW_WIDTH, WINDOW_HEIGHT},
      device{this->window},
      renderer{this->window, this->device} {}

Engine::~Engine() {}

void Engine::run() {
  while (!this->window.shouldClose()) { glfwPollEvents(); }
}

}  // namespace hep
