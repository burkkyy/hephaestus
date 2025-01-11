/**
 * @file main.cpp
 * @author Caleb Burke
 * @date Dec 13, 2024
 */

#include <iostream>
#include <stdexcept>

#include "src/device.hpp"
#include "src/pipeline.hpp"
#include "src/swapchain.hpp"
#include "src/window.hpp"

int main(int argc, const char** argv) {
  (void)argc;
  (void)argv;
  std::cout << __FILE__ << "::" << __LINE__ << '\n';

  hep::Window win(200, 200);
  hep::Device device(win);
  hep::Swapchain swapchain(device, win.getExtent());
  hep::Pipeline pipeline(device);
  pipeline.create("shaders/triangle.vert.spv", "shaders/triangle.frag.spv");

  while (!win.shouldClose()) { glfwPollEvents(); }

  return EXIT_SUCCESS;
}
