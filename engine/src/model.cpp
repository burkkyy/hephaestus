#include "model.hpp"

#include <cassert>

#include "util/logger.hpp"

namespace alp {

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
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};

  attributeDescriptions.push_back(
      {0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)});
  attributeDescriptions.push_back(
      {1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)});
  // attributeDescriptions.push_back(
  //     {2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)});
  // attributeDescriptions.push_back(
  //     {3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)});

  return attributeDescriptions;
}

Model::Model(Device& device, const Builder& builder) : device{device} {
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indicies);
}

Model::~Model() {}

void Model::bind(vk::CommandBuffer commandBuffer) {
  vk::Buffer buffers[] = {this->vertexBuffer->getBuffer()};
  vk::DeviceSize offset[] = {0};
  commandBuffer.bindVertexBuffers(0, 1, buffers, offset);

  if (this->hasIndexBuffer) {
    commandBuffer.bindIndexBuffer(this->indexBuffer->getBuffer(), 0,
                                  vk::IndexType::eUint32);
  }
}

void Model::draw(vk::CommandBuffer commandBuffer) {
  if (this->hasIndexBuffer) {
    commandBuffer.drawIndexed(this->indexCount, 1, 0, 0, 0);
  } else {
    commandBuffer.draw(this->vertexCount, 1, 0, 0);
  }
}

void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
  this->vertexCount = static_cast<u32>(vertices.size());
  assert(this->vertexCount >= 3 && "vertex count must be at least 3");

  vk::DeviceSize bufferSize = sizeof(vertices[0]) * this->vertexCount;

  u32 vertexSize = sizeof(vertices[0]);

  Buffer stagingBuffer{
      this->device,
      vertexSize,
      vertexCount,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent,
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void*)vertices.data());

  vertexBuffer =
      std::make_unique<Buffer>(this->device, vertexSize, vertexCount,
                               vk::BufferUsageFlagBits::eTransferDst |
                                   vk::BufferUsageFlagBits::eVertexBuffer,
                               vk::MemoryPropertyFlagBits::eDeviceLocal);

  this->device.copyBuffer(stagingBuffer.getBuffer(),
                          this->vertexBuffer->getBuffer(), bufferSize);
}

void Model::createIndexBuffers(const std::vector<u32>& indicies) {
  this->indexCount = static_cast<u32>(indicies.size());
  this->hasIndexBuffer = this->indexCount > 0;

  if (!this->hasIndexBuffer) { return; }

  vk::DeviceSize bufferSize = sizeof(indicies[0]) * this->indexCount;

  u32 indexSize = sizeof(indicies[0]);

  Buffer stagingBuffer{
      this->device,
      indexSize,
      indexCount,
      vk::BufferUsageFlagBits::eTransferSrc,
      vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent,
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void*)indicies.data());

  indexBuffer =
      std::make_unique<Buffer>(this->device, indexSize, indexCount,
                               vk::BufferUsageFlagBits::eTransferDst |
                                   vk::BufferUsageFlagBits::eIndexBuffer,
                               vk::MemoryPropertyFlagBits::eDeviceLocal);

  this->device.copyBuffer(stagingBuffer.getBuffer(),
                          this->indexBuffer->getBuffer(), bufferSize);
}

}  // namespace alp
