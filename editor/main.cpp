#include <imgui.h>

#include <alphane>
#include <iostream>
#include <stdexcept>

#include "debug_panel.hpp"

int main(int argc, const char** argv) {
  (void)argc;
  (void)argv;
  std::cout << __FILE__ << "::" << __LINE__ << '\n';

  alp::Application app{{.width = 750, .height = 1000, .name = "Hep"}};

  app.registerPanel(std::make_unique<DebugPanel>());

  app.addQuad({0.0f, 0.0f}, .2, .2);
  app.addQuad({-1.0f, -1.0f}, .2, .2);
  app.addQuad({.0f, -1.0f}, .2, .2);
  app.addQuad({-1, 0}, .1, .1);

  app.run();

  return EXIT_SUCCESS;
}
