#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "core/device.hpp"

namespace hep {

class DescriptorSetLayout {
 public:
  DescriptorSetLayout(const DescriptorSetLayout&) = delete;
  DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

  class Builder {
   public:
    Builder(Device& device) : device{device} {}

    Builder& addBinding(u32 binding,
                        vk::DescriptorType descriptorType,
                        vk::ShaderStageFlags stageFlags,
                        u32 count = 1);

    std::unique_ptr<DescriptorSetLayout> build() const;

   private:
    Device& device;
    std::unordered_map<u32, vk::DescriptorSetLayoutBinding> bindings{};
  };

  DescriptorSetLayout(
      Device& device,
      std::unordered_map<u32, vk::DescriptorSetLayoutBinding> bindings);
  ~DescriptorSetLayout();

  vk::DescriptorSetLayout getDescriptorSetLayout() const {
    return this->layout;
  }

 private:
  Device& device;
  vk::DescriptorSetLayout layout;
  std::unordered_map<u32, vk::DescriptorSetLayoutBinding> bindings;

  friend class DescriptorWriter;
};

}  // namespace hep
