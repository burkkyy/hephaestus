/**
 * @file engine/core/hephaestus.hpp
 * @author Caleb Burke
 * @date Jan 21, 2024
 */

#pragma once

#include "vulkan_plugin/plugin.hpp"

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
};

}	// namespace hep

