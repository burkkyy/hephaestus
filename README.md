# hephaestus

Cross platform general purpose GPU engine designed for real-time graphics applications

## Developing on the engine

### Dependencies

- [cmake](https://cmake.org/)
- [Vulkan SDK](https://vulkan.lunarg.com/) - With validation layers

> Quick Install for Ubuntu
>
> ```sh
> sudo apt install cmake vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools
> ```

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
