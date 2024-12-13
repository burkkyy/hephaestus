/**
 * @file engine/vulkan_plugin/pipeline.hpp
 * @author Caleb Burke
 * @date March 2, 2024
 * 
 * Graphics Pipeline Stages:
 * Vertex Input
 * Input assembler
 * Vertex Shader
 * Tessellation
 * Geometry Shader
 * Rasterization
 * Fragment Shader
 * Color Blendering
 */

#pragma once

#include "device.hpp"
#include "../util/types.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace hep {
namespace vul {

class Pipeline {
public:
    // Prevents copying and moving
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    Pipeline(Device& device, std::vector<char> vertex_shader_spv, std::vector<char> fragment_shader_spv);
    ~Pipeline();

private:
    void initialize();
    void create_pipeline();

    void default_config();
    void default_vertex_input();
    void default_input_assembly();
    void default_tessellation();
    void default_viewport();
    void default_rasterization();
    void default_multisample();
    void default_depth_stencil();
    void default_color_blend();
    void default_dynamic_state();

    VkShaderModule create_shader_module(const std::vector<char>& code);

    Device& device;
    VkPipeline pipeline;
    VkGraphicsPipelineCreateInfo pipeline_create_info;
    VkPipelineVertexInputStateCreateInfo vertex_input_create_info;
    VkPipelineInputAssemblyStateCreateInfo input_assemly_create_info;
    VkPipelineViewportStateCreateInfo viewport_state_create_info;
    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;
    VkPipelineMultisampleStateCreateInfo multisample_state_create_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info;
    VkPipelineColorBlendAttachmentState color_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info;
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info;

    VkShaderModule vertex_shader;
    VkShaderModule fragment_shader;
};

} // namespace vul
} // namespace hep

