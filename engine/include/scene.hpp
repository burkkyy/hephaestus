#pragma once

#include <entt/entt.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace alp {

/**
 * Class for storing a camera and a registry of entitys for rendering
 *
 * @note ASSUMING RENDER TARGET IS MAIN VIEWPORT (DISPLAYED WITH swapchain khr)
 * @note Camera not implemented at all yet
 */
class Scene {
 public:
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;

  Scene() = default;
  ~Scene() = default;

  virtual void onAttach();
  virtual void onUpdate();
  virtual void onRender();
  virtual void onImGuiRender();
  virtual void onDetach();

  void createQuad(glm::vec2 position, float width, float height);

 private:
  entt::registry registry;
};

}  // namespace alp
