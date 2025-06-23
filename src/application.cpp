#include <hep/application.hpp>  // SDK
#include <iostream>

namespace hep {

Application::Application(const ApplicationConfig& config) : config{config} {}

const ApplicationConfig& Application::getConfig() const { return this->config; }

std::shared_ptr<Engine> Application::getEngine() const {
  return this->engine.lock();
}

void Application::bindEngine(std::shared_ptr<Engine> engine) {
  this->engine = engine;
}

}  // namespace hep
