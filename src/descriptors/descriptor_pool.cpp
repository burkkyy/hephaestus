#include "descriptor_pool.hpp"

namespace hep {

DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(
    vk::DescriptorType descriptorType, u32 count) {
  this->poolSizes.push_back({descriptorType, count});
  return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(
    vk::DescriptorPoolCreateFlags flags) {
  this->poolFlags = flags;
  return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(u32 count) {
  this->maxSets = count;
  return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
  return std::make_unique<DescriptorPool>(this->device, this->maxSets,
                                          this->poolFlags, this->poolSizes);
}

DescriptorPool::DescriptorPool(
    Device& device, u32 maxSets, vk::DescriptorPoolCreateFlags poolFlags,
    const std::vector<vk::DescriptorPoolSize>& poolSizes)
    : device{device} {
  vk::DescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();
  descriptorPoolInfo.maxSets = maxSets;
  descriptorPoolInfo.flags = poolFlags;

  vk::Result result = this->device.get()->createDescriptorPool(
      &descriptorPoolInfo, nullptr, &this->descriptorPool);

  if (result != vk::Result::eSuccess) {
    log::fatal("failed to create descriptor pool");
    throw std::runtime_error("failed to create descriptor pool");
  }
}

DescriptorPool::~DescriptorPool() {
  this->device.get()->destroyDescriptorPool(this->descriptorPool);
}

bool DescriptorPool::allocateDescriptorSet(
    const vk::DescriptorSetLayout descriptorSetLayout,
    vk::DescriptorSet& descriptor) const {
  vk::DescriptorSetAllocateInfo allocInfo{};
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.pSetLayouts = &descriptorSetLayout;
  allocInfo.descriptorSetCount = 1;

  // Might want to create a "DescriptorPoolManager" class that handles this
  // case, and builds a new pool whenever an old pool fills up. But this is
  // beyond our current scope
  vk::Result result =
      this->device.get()->allocateDescriptorSets(&allocInfo, &descriptor);

  if (result == vk::Result::eSuccess) { return true; }

  return false;
}

void DescriptorPool::freeDescriptors(
    std::vector<vk::DescriptorSet>& descriptors) const {
  this->device.get()->freeDescriptorSets(this->descriptorPool,
                                         static_cast<u32>(descriptors.size()),
                                         descriptors.data());
}

void DescriptorPool::resetPool() {
  this->device.get()->resetDescriptorPool(this->descriptorPool);
}

}  // namespace hep
