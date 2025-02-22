#pragma once

#include <vector>

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace hep {

class Model {
 public:
  struct Vertex {
    glm::vec2 position{};
    glm::vec3 color{};
    // glm::vec3 normal{};
    // glm::vec2 uv{};

    static std::vector<vk::VertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<vk::VertexInputAttributeDescription>
    getAttributeDescriptions();

    // bool operator==(const Vertex& other) const {
    //   return position == other.position && color == other.color &&
    //          normal == other.normal && uv == other.uv;
    // }
  };

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  Model(Device& device, const std::vector<Vertex>& vertices);
  ~Model();

  void bind(vk::CommandBuffer commandBuffer);
  void draw(vk::CommandBuffer commandBuffer);

 private:
  void createVertexBuffers(const std::vector<Vertex>& vertices);

  Device& device;
  vk::Buffer vertexBuffer;
  vk::DeviceMemory vertexBufferMemory;
  u32 vertexCount;
};

}  // namespace hep
