#pragma once

#include <memory>
#include <vector>

#include "rendering/buffer.hpp"
#include "rendering/core/device.hpp"

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

  /**
   * Helper struct
   *
   * Temporarily stores vertices and indicies of a model before it
   * can be moved into the model's vertex buffer and index buffer respectively
   *
   * TODO: move this documentation elsewhere (doc/ ?)
   */
  struct Builder {
    std::vector<Vertex> vertices{};
    std::vector<u32> indicies{};
  };

  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;

  Model(Device& device, const Builder& builder);
  ~Model();

  void bind(vk::CommandBuffer commandBuffer);
  void draw(vk::CommandBuffer commandBuffer);

 private:
  void createVertexBuffers(const std::vector<Vertex>& vertices);
  void createIndexBuffers(const std::vector<u32>& indicies);

  Device& device;

  std::unique_ptr<Buffer> vertexBuffer;
  u32 vertexCount;

  bool hasIndexBuffer = false;
  std::unique_ptr<Buffer> indexBuffer;
  u32 indexCount;
};

}  // namespace hep
