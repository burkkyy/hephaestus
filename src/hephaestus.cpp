#include <hephaestus>  // SDK

#include "util/logger.hpp"

namespace hep {

Hephaestus::Hephaestus(std::unique_ptr<Application> app)
    : pImpl(std::make_unique<Impl>()), app{std::move(app)} {
  this->config = this->app->getConfig();
  this->engine = std::make_shared<Engine>(this->config);
  this->app->bindEngine(this->engine);
}

Hephaestus::~Hephaestus() = default;

void Hephaestus::launch() {
  if (this->app == nullptr) { return; }

  this->app->onStart();
  if (!this->app->config.headless) { this->engine->run(); }
  this->app->onClose();
}

class Hephaestus::Impl {
 public:
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;

  Impl() = default;
  ~Impl() = default;
};

}  // namespace hep
