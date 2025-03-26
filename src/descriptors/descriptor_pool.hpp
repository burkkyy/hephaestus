#pragma once

#include <memory>
#include <vector>

#include "device.hpp"

namespace hep {

class DescriptorPool {
 public:
  DescriptorPool(const DescriptorPool&) = delete;
  DescriptorPool& operator=(const DescriptorPool&) = delete;

  class Builder {
   public:
    Builder(Device& device) : device{device} {}

    Builder& addPoolSize(vk::DescriptorType descriptorType, u32 count);
    Builder& setPoolFlags(vk::DescriptorPoolCreateFlags flags);
    Builder& setMaxSets(u32 count);
    std::unique_ptr<DescriptorPool> build() const;

   private:
    Device& device;
    std::vector<vk::DescriptorPoolSize> poolSizes{};
    u32 maxSets = 1000;
    vk::DescriptorPoolCreateFlags poolFlags{};
  };

  DescriptorPool(Device& device, u32 maxSets,
                 vk::DescriptorPoolCreateFlags poolFlags,
                 const std::vector<vk::DescriptorPoolSize>& poolSizes);
  ~DescriptorPool();

  bool allocateDescriptorSet(const vk::DescriptorSetLayout descriptorSetLayout,
                             vk::DescriptorSet& descriptor) const;

  void freeDescriptors(std::vector<vk::DescriptorSet>& descriptors) const;

  void resetPool();

  const vk::DescriptorPool& get() { return this->descriptorPool; }

 private:
  Device& device;
  vk::DescriptorPool descriptorPool;

  friend class DescriptorWriter;
};

}  // namespace hep
