#include "descriptor_writer.hpp"

#include <cassert>
#include <stdexcept>

namespace alp {

DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout,
                                   DescriptorPool& pool)
    : setLayout{setLayout}, pool{pool} {}

DescriptorWriter& DescriptorWriter::writeBuffer(
    u32 binding,
    vk::DescriptorBufferInfo* bufferInfo) {
  assert(this->setLayout.bindings.count(binding) == 1 &&
         "setLayout does not contain specified binding");

  auto& bindingDescription = this->setLayout.bindings[binding];

  assert(bindingDescription.descriptorCount == 1 &&
         "binding single descriptor info, but binding expects multiple");

  vk::WriteDescriptorSet write{};
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = bufferInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(
    u32 binding,
    vk::DescriptorImageInfo* imageInfo) {
  assert(this->setLayout.bindings.count(binding) == 1 &&
         "setLayout does not contain specified binding");

  auto& bindingDescription = this->setLayout.bindings[binding];

  assert(bindingDescription.descriptorCount == 1 &&
         "binding single descriptor info, but binding expects multiple");

  vk::WriteDescriptorSet write{};
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

bool DescriptorWriter::build(vk::DescriptorSet& set) {
  bool success =
      this->pool.allocateDescriptorSet(setLayout.getDescriptorSetLayout(), set);

  if (!success) { return false; }
  overwrite(set);
  return true;
}

void DescriptorWriter::overwrite(vk::DescriptorSet& set) {
  for (auto& write : writes) { write.dstSet = set; }

  this->pool.device.get()->updateDescriptorSets(writes.size(), writes.data(), 0,
                                                nullptr);
}

}  // namespace alp
