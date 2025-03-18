#include <iostream>
#include <stdexcept>

#include "hephaestus/engine.hpp"

int main(int argc, const char** argv) {
  (void)argc;
  (void)argv;
  std::cout << __FILE__ << "::" << __LINE__ << '\n';

  hep::Engine engine;
  engine.run();

  return EXIT_SUCCESS;
}
