#pragma once

#include <vector>

#include "core/device.hpp"
#include "descriptor_pool.hpp"
#include "descriptor_set_layout.hpp"

namespace hep {

class DescriptorWriter {
 public:
  DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

  DescriptorWriter& writeBuffer(u32 binding,
                                vk::DescriptorBufferInfo* bufferInfo);
  DescriptorWriter& writeImage(u32 binding, vk::DescriptorImageInfo* imageInfo);

  bool build(vk::DescriptorSet& set);
  void overwrite(vk::DescriptorSet& set);

 private:
  DescriptorSetLayout& setLayout;
  DescriptorPool& pool;
  std::vector<vk::WriteDescriptorSet> writes;
};

}  // namespace hep
