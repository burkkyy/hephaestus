#pragma once

#include <memory>

#include "scene.hpp"
#include "types.hpp"

#define MAX_SCENE_COUNT 2

namespace alp {

class SceneManager {
 public:
  SceneManager(const SceneManager&) = delete;
  SceneManager& operator=(const SceneManager&) = delete;

  SceneManager();
  ~SceneManager();

  void registerScene(std::unique_ptr<Scene> scene);

  void onAttachActiveScene();
  void onUpdateActiveScene();
  void onRenderActiveScene();
  void onImGuiRenderActiveScene();
  void onDetachActiveScene();

 private:
  // std::array<std::shared_ptr<Scene>, MAX_SCENE_COUNT> scenes;

  // Only supporting 1 scene
  std::unique_ptr<Scene> activeScene;
};

}  // namespace alp
