# hephaestus

Cross platform general purpose GPU engine designed for real-time graphics applications

## Usage

```cpp
#include <hephaestus>
#include <iostream>

class MyApplication : public hep::Application {
 public:
  MyApplication(hep::ApplicationConfig config) : hep::Application(config) {}

  void onStart(){
    std::cout << "Hello world!" < std::endl;
  }
};

int main() {
  hep::ApplicationConfig config {
    .width = 600,
    .height = 400,
    .name = "My Application"
  };

  hep::Hephaestus heph(std::make_unique<MyApplication>(config));
  heph.launch();

  return 0;
}
```

## Getting Started

COMING SOON

## Developing on the engine

### Dependencies

- [cmake](https://cmake.org/)
- [Vulkan SDK](https://vulkan.lunarg.com/) - With validation layers

### Build instructions

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cd build
make -j
```

## Long term goals

- Buffered event system (oppesed to current blocking event system)
- Full ECS implementation

## Resources

- [Vulkan tutorial playlist](https://www.youtube.com/watch?v=Y9U9IE0gVHA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR)
- [Vulkan Specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html)
- [vulkan-tutorial](https://vulkan-tutorial.com/)
- [vkguide](https://vkguide.dev/)
- [Computer Graphics at TU Wien - Vulkan](https://www.youtube.com/playlist?list=PLmIqTlJ6KsE0UsR2E_84-twxX6G7ynZNq)
