#include "pipeline.hpp"

#include <fstream>

#include "util/logger.hpp"

namespace hep {

Pipeline::Pipeline(Device& device) : device{device} {}

Pipeline::~Pipeline() {}

void Pipeline::create(const std::string& vertexShaderFilename,
                      const std::string& fragmentShaderFilename) {
  vk::UniqueShaderModule vertexShaderModule =
      createShaderModule(vertexShaderFilename);
  log::verbose("created vertex shader module");

  vk::UniqueShaderModule fragmentShaderModule =
      createShaderModule(fragmentShaderFilename);
  log::verbose("created fragment shader module");

  vk::PipelineShaderStageCreateInfo shaderStages[] = {
      {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex,
       vertexShaderModule.get(), "main"},
      {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment,
       fragmentShaderModule.get(), "main"}};
}

vk::UniqueShaderModule Pipeline::createShaderModule(
    const std::string& shaderFilename) {
  std::ifstream file(shaderFilename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    log::error("failed to open: " + shaderFilename);
    throw std::runtime_error("failed to open: " + shaderFilename);
  }

  size_t fileSize = (size_t)file.tellg();

  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  try {
    return device.get()->createShaderModuleUnique(
        {vk::ShaderModuleCreateFlags(), buffer.size(),
         reinterpret_cast<const uint32_t*>(buffer.data())});
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create shader module");
    throw std::runtime_error("failed to create shader module");
  }
}

}  // namespace hep
