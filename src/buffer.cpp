/**
 * Based on
 * https://github.com/blurrypiano/littleVulkanEngine/blob/main/src/lve_buffer.cpp
 */
#include "buffer.hpp"

namespace hep {

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize,
                                  VkDeviceSize minOffsetAlignment) {
  if (minOffsetAlignment > 0) {
    return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
  }
  return instanceSize;
}

Buffer::Buffer(Device& device, vk::DeviceSize instanceSize, u32 instanceCount,
               vk::BufferUsageFlags usageFlags,
               vk::MemoryPropertyFlags memoryPropertyFlags,
               vk::DeviceSize minOffsetAlignment)
    : device{device},
      instanceSize{instanceSize},
      instanceCount{instanceCount},
      usageFlags{usageFlags},
      memoryPropertyFlags{memoryPropertyFlags} {
  alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
  bufferSize = alignmentSize * instanceCount;
  device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer,
                      memory);
}

Buffer::~Buffer() {
  unmap();
  this->device.get()->destroyBuffer(this->buffer);
  this->device.get()->freeMemory(this->memory);
}

vk::Result Buffer::map(vk::DeviceSize size, vk::DeviceSize offset) {
  assert(buffer && memory && "Called map on buffer before create");

  vk::MemoryMapFlags emptyFlag{};
  return this->device.get()->mapMemory(this->memory, offset, size, emptyFlag,
                                       &this->mapped);
}

void Buffer::unmap() {
  if (mapped) {
    this->device.get()->unmapMemory(this->memory);
    mapped = nullptr;
  }
}

void Buffer::writeToBuffer(void* data, vk::DeviceSize size,
                           vk::DeviceSize offset) {
  assert(mapped && "Cannot copy to unmapped buffer");

  if (size == VK_WHOLE_SIZE) {
    memcpy(mapped, data, bufferSize);
  } else {
    char* memOffset = (char*)mapped;
    memOffset += offset;
    memcpy(memOffset, data, size);
  }
}

vk::Result Buffer::flush(vk::DeviceSize size, vk::DeviceSize offset) {
  vk::MappedMemoryRange mappedRange = {};
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return this->device.get()->flushMappedMemoryRanges(1, &mappedRange);
}

vk::Result Buffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset) {
  vk::MappedMemoryRange mappedRange = {};
  mappedRange.memory = memory;
  mappedRange.offset = offset;
  mappedRange.size = size;
  return this->device.get()->invalidateMappedMemoryRanges(1, &mappedRange);
}

vk::DescriptorBufferInfo Buffer::descriptorInfo(vk::DeviceSize size,
                                                vk::DeviceSize offset) {
  return vk::DescriptorBufferInfo{
      buffer,
      offset,
      size,
  };
}

void Buffer::writeToIndex(void* data, int index) {
  writeToBuffer(data, instanceSize, index * alignmentSize);
}

vk::Result Buffer::flushIndex(int index) {
  return flush(alignmentSize, index * alignmentSize);
}

vk::DescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
  return descriptorInfo(alignmentSize, index * alignmentSize);
}

vk::Result Buffer::invalidateIndex(int index) {
  return invalidate(alignmentSize, index * alignmentSize);
}

}  // namespace hep
