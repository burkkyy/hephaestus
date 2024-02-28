/**
 * @file engine/core/hephaestus.cpp
 * @author Caleb Burke
 * @date Jan 21, 2024
 */

#include "hephaestus.hpp"

namespace hep {

Instance::Instance(){
	initialize();
}

Instance::~Instance(){

}

void Instance::initialize(){

}

void Instance::run(){
	vul::Window window(500, 500);
	vul::Device device(window);
	vul::Swapchain swapchain(device, window.get_extent());

	while(!window.should_close()){
		glfwPollEvents();
	}
}

}	// namespace hep

