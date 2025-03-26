#include "descriptor_set_layout.hpp"

#include <cassert>
#include <stdexcept>

namespace hep {

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(
    u32 binding, vk::DescriptorType descriptorType,
    vk::ShaderStageFlags stageFlags, u32 count) {
  assert(this->bindings.count(binding) == 0 && "binding already in use");

  vk::DescriptorSetLayoutBinding layoutBinding{binding, descriptorType, count,
                                               stageFlags};
  this->bindings[binding] = layoutBinding;

  return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build()
    const {
  return std::make_unique<DescriptorSetLayout>(this->device, this->bindings);
}

DescriptorSetLayout::DescriptorSetLayout(
    Device& Device,
    std::unordered_map<u32, vk::DescriptorSetLayoutBinding> bindings)
    : device{device}, bindings{bindings} {
  std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings{};

  for (auto binding : bindings) { setLayoutBindings.push_back(binding.second); }

  vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{};
  layoutCreateInfo.bindingCount =
      static_cast<uint32_t>(setLayoutBindings.size());
  layoutCreateInfo.pBindings = setLayoutBindings.data();

  vk::Result result = this->device.get()->createDescriptorSetLayout(
      &layoutCreateInfo, nullptr, &this->layout);

  if (result != vk::Result::eSuccess) {
    log::fatal("failed to create descriptor pool");
    throw std::runtime_error("failed to create descriptor set layout");
  }
}

DescriptorSetLayout::~DescriptorSetLayout() {
  this->device.get()->destroyDescriptorSetLayout(this->layout);
}

}  // namespace hep
