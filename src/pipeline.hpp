#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "device.hpp"

namespace hep {

class Pipeline {
 public:
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  Pipeline(Device& device);
  ~Pipeline();

  void create(const std::string& vertexShaderFilename,
              const std::string& fragmentShaderFilename);

 private:
  vk::UniqueShaderModule createShaderModule(const std::string& shaderFilename);

  Device& device;
};

}  // namespace hep
