#pragma once

#include <memory>

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
  class Impl;
  std::unique_ptr<Impl> pImpl;
};

}  // namespace hep
