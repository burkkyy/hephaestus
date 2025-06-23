#pragma once

#include <memory>

namespace hep {

struct ApplicationConfig;

class Engine {
 public:
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  Engine(const ApplicationConfig& config);
  ~Engine();

  void run();

 private:
  class Impl;
  std::unique_ptr<Impl> pImpl;
};

}  // namespace hep
