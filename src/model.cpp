#include "model.hpp"

#include <cassert>

#include "util/logger.hpp"

namespace hep {

std::vector<vk::VertexInputBindingDescription>
Model::Vertex::getBindingDescriptions() {
  std::vector<vk::VertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

  return bindingDescriptions;
}

std::vector<vk::VertexInputAttributeDescription>
Model::Vertex::getAttributeDescriptions() {
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(1);

  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
  attributeDescriptions[0].offset = 0;

  // attributeDescriptions.push_back(
  //     {0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)});
  // attributeDescriptions.push_back(
  //     {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)});
  // attributeDescriptions.push_back(
  //     {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)});
  // attributeDescriptions.push_back(
  //     {3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)});

  return attributeDescriptions;
}

Model::Model(Device& device, const std::vector<Vertex>& vertices)
    : device{device} {
  createVertexBuffers(vertices);
}

Model::~Model() {
  this->device.get()->destroyBuffer(this->vertexBuffer);
  log::verbose("destroyed vertexBuffer");

  this->device.get()->freeMemory(this->vertexBufferMemory);
  log::verbose("freed memory for vertexBuffer");
}

void Model::bind(vk::CommandBuffer commandBuffer) {
  vk::Buffer buffers[] = {this->vertexBuffer};
  vk::DeviceSize offset[] = {0};
  commandBuffer.bindVertexBuffers(0, 1, buffers, offset);
}

void Model::draw(vk::CommandBuffer commandBuffer) {
  commandBuffer.draw(this->vertexCount, 1, 0, 0);
}

void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
  this->vertexCount = static_cast<u32>(vertices.size());
  assert(this->vertexCount >= 3 && "vertex count must be at least 3");

  vk::DeviceSize bufferSize = sizeof(vertices[0]) * this->vertexCount;

  this->device.createBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer,
                            vk::MemoryPropertyFlagBits::eHostVisible |
                                vk::MemoryPropertyFlagBits::eHostCoherent,
                            this->vertexBuffer, this->vertexBufferMemory);

  void* data = this->device.get()->mapMemory(vertexBufferMemory, 0, bufferSize);
  memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
  this->device.get()->unmapMemory(vertexBufferMemory);
}

}  // namespace hep
