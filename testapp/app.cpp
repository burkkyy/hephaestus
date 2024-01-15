/**
 * @file testapp/app.cpp
 * @author Caleb Burke
 * @date Jan 6, 2024
 */

#include <iostream>

#include <core/test.hpp>
#include <vulkan/window.hpp>
#include <vulkan/device.hpp>

int main(int argc, const char** argv) {
	(void)argc;
	(void)argv;
	std::cout << __FILE__ << "::" << __LINE__ << '\n';
    
	bool b = testprint(123);
	if(b){
		std::cout << "yes\n";
	} else {
		std::cout << "no\n";
	}

	hep::Window win(200, 200, "test");
	hep::Device device(win);		
	return 0;
}

