#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "device.hpp"
#include "types.hpp"

namespace alp {

struct PipelineConfig {
  PipelineConfig() = default;
  PipelineConfig(const PipelineConfig&) = delete;
  PipelineConfig& operator=(const PipelineConfig&) = delete;

  std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
  vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
  vk::PipelineMultisampleStateCreateInfo multisampleInfo;
  vk::PipelineColorBlendAttachmentState colorBlendAttachment;
  vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
  vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<vk::DynamicState> dynamicStateEnables;
  vk::PipelineDynamicStateCreateInfo dynamicStateInfo;

  vk::PipelineLayout pipelineLayout = VK_NULL_HANDLE;
  vk::RenderPass renderPass = VK_NULL_HANDLE;
  u32 subpass = 0;
};

class Pipeline {
 public:
  Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  Pipeline(Device& device);
  ~Pipeline();

  void create(const std::string& vertexShaderFilename,
              const std::string& fragmentShaderFilename,
              vk::PipelineLayout pipelineLayout,
              vk::RenderPass renderPass);

  void create(unsigned char* vertexShaderSpv,
              u32 vertexShaderSpvLen,
              unsigned char* fragmentShaderSpv,
              u32 fragmentShaderSpvLen,
              vk::PipelineLayout pipelineLayout,
              vk::RenderPass renderPass);

  void bind(vk::CommandBuffer commandBuffer);

 private:
  void setDefaultPipelineConfig();
  vk::UniqueShaderModule createShaderModule(const std::string& shaderFilename);
  vk::UniqueShaderModule createShaderModule(unsigned char* shaderData,
                                            u32 shaderDataLen);

  Device& device;
  PipelineConfig config;
  vk::Pipeline graphicsPipeline;
};

}  // namespace alp
