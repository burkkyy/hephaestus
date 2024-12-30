/**
 * @file main.cpp
 * @author Caleb Burke
 * @date Dec 13, 2024
 */

#include <iostream>
#include <stdexcept>

#include "src/window.hpp"
#include "src/device.hpp"

int main(int argc, const char** argv) {
	(void)argc;
	(void)argv;
	std::cout << __FILE__ << "::" << __LINE__ << '\n';

	hep::Window win(200, 200);
	hep::Device device(win);

	while(!win.shouldClose()) {
    glfwPollEvents();
  }

	return EXIT_SUCCESS;
}
