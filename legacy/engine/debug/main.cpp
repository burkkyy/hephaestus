/** 
 * @file engine/debug/main.cpp
 * @author Caleb Burke
 * @date March 2, 2024
 */

#include <core/hephaestus.hpp>
#include <exception>
#include <iostream>

int main(int argc, const char** argv){
	(void)argc;
	(void)argv;
    
    hep::Instance instance;
    
    try {
        instance.run();
    } catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}

