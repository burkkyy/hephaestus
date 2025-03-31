#include "pipeline.hpp"

#include <fstream>

#include "rendering/model.hpp"
#include "util/logger.hpp"

namespace hep {

Pipeline::Pipeline(Device& device) : device{device} {
  setDefaultPipelineConfig();
}

Pipeline::~Pipeline() {
  log::trace("destoryed vk::Pipeline");
  this->device.get()->destroyPipeline(this->graphicsPipeline);
}

void Pipeline::create(const std::string& vertexShaderFilename,
                      const std::string& fragmentShaderFilename,
                      vk::PipelineLayout pipelineLayout,
                      vk::RenderPass renderPass) {
  if (renderPass == VK_NULL_HANDLE) {
    log::fatal(
        "failed to create graphics pipeline: no vk::RenderPass provided");
    throw std::runtime_error(
        "failed to create graphics pipeline: no vk::RenderPass provided");
  }

  if (pipelineLayout == VK_NULL_HANDLE) {
    log::fatal(
        "failed to create graphics pipeline: no vk::PipelineLayout provided");
    throw std::runtime_error(
        "failed to create graphics pipeline: no vk::PipelineLayout provided");
  }

  if (this->graphicsPipeline) {
    log::trace("destorying old vk::Pipeline");
    this->device.get()->destroyPipeline(this->graphicsPipeline);
  }

  vk::UniqueShaderModule vertexShaderModule =
      createShaderModule(vertexShaderFilename);
  log::trace("created vertex shader module");

  vk::UniqueShaderModule fragmentShaderModule =
      createShaderModule(fragmentShaderFilename);
  log::trace("created fragment shader module");

  vk::PipelineShaderStageCreateInfo shaderStages[] = {
      {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex,
       vertexShaderModule.get(), "main"},
      {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment,
       fragmentShaderModule.get(), "main"}};

  auto bindingDescriptions = Model::Vertex::getBindingDescriptions();
  auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.vertexBindingDescriptionCount =
      static_cast<u32>(bindingDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<u32>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  vk::PipelineViewportStateCreateInfo viewportInfo = {};
  viewportInfo.viewportCount = 1;
  viewportInfo.pViewports = nullptr;
  viewportInfo.scissorCount = 1;
  viewportInfo.pScissors = nullptr;

  vk::GraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &this->config.inputAssemblyInfo;
  pipelineInfo.pViewportState = &viewportInfo;
  pipelineInfo.pRasterizationState = &this->config.rasterizationInfo;
  pipelineInfo.pMultisampleState = &this->config.multisampleInfo;
  pipelineInfo.pDepthStencilState = &this->config.depthStencilInfo;
  pipelineInfo.pColorBlendState = &this->config.colorBlendInfo;
  pipelineInfo.pDynamicState = &this->config.dynamicStateInfo;

  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;

  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = nullptr;

  try {
    this->graphicsPipeline =
        this->device.get()->createGraphicsPipeline(nullptr, pipelineInfo).value;
    log::trace("created vk::Pipeline");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::Pipeline");
    throw std::runtime_error("failed to create vk::Pipeline");
  }
}

void Pipeline::bind(vk::CommandBuffer commandBuffer) {
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             this->graphicsPipeline);
}

void Pipeline::setDefaultPipelineConfig() {
  this->config.inputAssemblyInfo.topology =
      vk::PrimitiveTopology::eTriangleList;
  this->config.inputAssemblyInfo.primitiveRestartEnable = vk::False;

  this->config.rasterizationInfo.depthClampEnable = vk::False;
  this->config.rasterizationInfo.rasterizerDiscardEnable = vk::False;
  this->config.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
  this->config.rasterizationInfo.lineWidth = 1.0f;
  this->config.rasterizationInfo.cullMode = vk::CullModeFlagBits::eNone;
  this->config.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
  this->config.rasterizationInfo.depthBiasEnable = vk::False;
  this->config.rasterizationInfo.depthBiasConstantFactor = 0.0f;
  this->config.rasterizationInfo.depthBiasClamp = 0.0f;
  this->config.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

  this->config.multisampleInfo.sampleShadingEnable = vk::False;
  this->config.multisampleInfo.rasterizationSamples =
      vk::SampleCountFlagBits::e1;
  this->config.multisampleInfo.minSampleShading = 1.0f;
  this->config.multisampleInfo.pSampleMask = nullptr;
  this->config.multisampleInfo.alphaToCoverageEnable = vk::False;
  this->config.multisampleInfo.alphaToOneEnable = vk::False;

  this->config.colorBlendAttachment.colorWriteMask =
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
  this->config.colorBlendAttachment.blendEnable = vk::False;
  this->config.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
  this->config.colorBlendAttachment.dstColorBlendFactor =
      vk::BlendFactor::eZero;
  this->config.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
  this->config.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
  this->config.colorBlendAttachment.dstAlphaBlendFactor =
      vk::BlendFactor::eZero;
  this->config.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

  this->config.colorBlendInfo.logicOpEnable = vk::False;
  this->config.colorBlendInfo.logicOp = vk::LogicOp::eCopy;
  this->config.colorBlendInfo.attachmentCount = 1;
  this->config.colorBlendInfo.pAttachments = &this->config.colorBlendAttachment;
  this->config.colorBlendInfo.blendConstants[0] = 0.0f;
  this->config.colorBlendInfo.blendConstants[1] = 0.0f;
  this->config.colorBlendInfo.blendConstants[2] = 0.0f;
  this->config.colorBlendInfo.blendConstants[3] = 0.0f;

  this->config.depthStencilInfo.depthTestEnable = vk::True;
  this->config.depthStencilInfo.depthWriteEnable = vk::True;
  this->config.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
  this->config.depthStencilInfo.depthBoundsTestEnable = vk::False;
  this->config.depthStencilInfo.minDepthBounds = 0.0f;
  this->config.depthStencilInfo.maxDepthBounds = 1.0f;
  this->config.depthStencilInfo.stencilTestEnable = vk::False;

  this->config.dynamicStateEnables = {vk::DynamicState::eViewport,
                                      vk::DynamicState::eScissor};
  this->config.dynamicStateInfo.pDynamicStates =
      this->config.dynamicStateEnables.data();
  this->config.dynamicStateInfo.dynamicStateCount =
      static_cast<u32>(this->config.dynamicStateEnables.size());
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
         reinterpret_cast<const u32*>(buffer.data())});
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create shader module");
    throw std::runtime_error("failed to create shader module");
  }
}

}  // namespace hep
