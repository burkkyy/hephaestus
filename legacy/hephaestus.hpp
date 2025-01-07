/**
 * @file engine/core/hephaestus.hpp
 * @author Caleb Burke
 * @date Jan 21, 2024
 */

#pragma once

#include <vector>
#include <string>

namespace hep {

class Instance {
	// Prevents copying and moving
	Instance(const Instance&) = delete;
	Instance& operator=(const Instance&) = delete;
	Instance(Instance&&) = delete;
	Instance& operator=(Instance&&) = delete;

public:
	Instance();
	~Instance();

	void run();

private:
	void initialize();
    void initialize_vulkan();

    std::vector<char> read_shader_file(const std::string& filepath);
    //void compile_shader(const std::string& filepath);
};

}	// namespace hep

