#pragma once

namespace hep {

class Panel {
 public:
  Panel(const Panel&) = delete;
  Panel& operator=(const Panel&) = delete;

  Panel() = default;
  ~Panel() = default;

  virtual void onUpdate();
};

}  // namespace hep
