#pragma once

#include <memory>
#include <string>

namespace hep {

class Engine;
class Hephaestus;

struct ApplicationConfig {
  bool headless = false;
  int width;
  int height;
  std::string name = "Hep";
};

class Application {
 public:
  explicit Application(const ApplicationConfig& config);
  virtual ~Application() = default;

 protected:
  virtual void onStart() {}
  virtual void onRender(float deltaTime) {}
  virtual void onClose() {}

  const ApplicationConfig& getConfig() const;
  std::shared_ptr<Engine> getEngine() const;

 private:
  void bindEngine(std::shared_ptr<Engine> engine);

  ApplicationConfig config;
  std::weak_ptr<Engine> engine;

  friend class Hephaestus;
};

}  // namespace hep
