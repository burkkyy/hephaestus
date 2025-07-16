#include "scene_manager.hpp"

namespace alp {

SceneManager::SceneManager() = default;

SceneManager::~SceneManager() = default;

void SceneManager::registerScene(std::unique_ptr<Scene> scene) {
  if (this->activeScene) { this->activeScene.reset(); }

  this->activeScene = std::move(scene);
}

void SceneManager::onAttachActiveScene() { this->activeScene->onAttach(); }

void SceneManager::onUpdateActiveScene() { this->activeScene->onUpdate(); }

void SceneManager::onRenderActiveScene() { this->activeScene->onRender(); }

void SceneManager::onImGuiRenderActiveScene() {
  this->activeScene->onImGuiRender();
}

void SceneManager::onDetachActiveScene() { this->activeScene->onDetach(); }

}  // namespace alp
