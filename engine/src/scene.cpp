#include "scene.hpp"

#include <iostream>

#include "components.hpp"

namespace alp {

void Scene::onAttach() {}
void Scene::onUpdate() {}
void Scene::onRender() {}
void Scene::onImGuiRender() {}

void Scene::onDetach() {
  auto view = this->registry.view<Quad2D>();

  std::cout << "Detaching scene - Found " << view.size() << " quads:\n";

  for (auto entity : view) {
    std::cout << "Entity ID: " << static_cast<uint32_t>(entity) << std::endl;
  }
}

void Scene::createQuad(glm::vec2 position, float width, float height) {
  entt::entity entity = this->registry.create();
  this->registry.emplace<Quad2D>(entity, width, height,
                                 glm::vec4{0, 0, 1.0, 1.0});
  this->registry.emplace<Transform2D>(entity, position);
}

}  // namespace alp
