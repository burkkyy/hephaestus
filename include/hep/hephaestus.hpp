#pragma once

#include <memory>

namespace hep {

class Application;
class Engine;

class Hephaestus {
 public:
  Hephaestus(const Hephaestus&) = delete;
  Hephaestus& operator=(const Hephaestus&) = delete;

  explicit Hephaestus(std::unique_ptr<Application> app);
  ~Hephaestus();

  void launch();

 private:
  std::unique_ptr<Application> app;
  ApplicationConfig config;
  std::shared_ptr<Engine> engine;

  class Impl;
  std::unique_ptr<Impl> pImpl;
};

}  // namespace hep
