/**
 * @file engine/core/hephaestus.cpp
 * @author Caleb Burke
 * @date Jan 21, 2024
 */

#include "hephaestus.hpp"

#include "util/logger.hpp"

#include "vulkan_plugin/window.hpp" 
#include "vulkan_plugin/device.hpp"
#include "vulkan_plugin/swapchain.hpp"
#include "vulkan_plugin/pipeline.hpp"

#include <exception>
#include <fstream>

namespace hep {

Instance::Instance(){

}

Instance::~Instance(){

}

void Instance::initialize(){
    initialize_vulkan();
}

void Instance::initialize_vulkan(){
}

void Instance::run(){
	vul::Window window(500, 500);
	vul::Device device(window);
	vul::Swapchain swapchain(device, window.get_extent());
    
    auto vertex_shader = read_shader_file("shader.vert.spv");
    auto fragment_shader = read_shader_file("shader.frag.spv");

    vul::Pipeline pipeline(device, vertex_shader, fragment_shader);

    while(!window.should_close()){
		glfwPollEvents();
	}
}

std::vector<char> Instance::read_shader_file(const std::string& filepath){
    std::ifstream f(filepath, std::ios::ate | std::ios::binary);
    if(!f.is_open()){ 
        log::error("Failed to open file: ", filepath);
        throw std::exception();
    }

    size_t size = (size_t)f.tellg();
    std::vector<char> buffer(size);
    
    f.seekg(0);
    f.read(buffer.data(), size);
    f.close();

    return buffer;
}

}	// namespace hep

