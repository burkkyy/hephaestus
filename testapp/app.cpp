/**
 * @file testapp/app.cpp
 * @author Caleb Burke
 * @date Jan 6, 2024
 */

#include <core/hephaestus.hpp>

#include <iostream>
#include <stdexcept>

int main(int argc, const char** argv) {
	(void)argc;
	(void)argv;
	std::cout << __FILE__ << "::" << __LINE__ << '\n';

	hep::Instance instance;

	try {
		instance.run();
	} catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

