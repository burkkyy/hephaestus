#pragma once

#include "frame_info.hpp"

namespace alp {

class Panel {
 public:
  Panel(const Panel&) = delete;
  Panel& operator=(const Panel&) = delete;

  Panel() = default;
  ~Panel() = default;

  virtual void onUpdate(const FrameInfo& frameInfo);
};

}  // namespace alp
