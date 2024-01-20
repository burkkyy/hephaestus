# hephaestus

## Build Instructions
```bash
./setup.sh
make
```
Engine found in `engine/build/lib`. To use the shared library, env variable `LD_LIBRARY_PATH` must be set so dynamic linker can find it:

```bash
source ./env.sh
```

## Dependencies
- `base-devel`
- `cmake`
- `vulkan-devel`

## Resources
- [vulkan-tutorial](https://vulkan-tutorial.com/)
- [Vulkan Specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html)
- [Computer Graphics at TU Wien - Vulkan](https://www.youtube.com/playlist?list=PLmIqTlJ6KsE0UsR2E_84-twxX6G7ynZNq)
- [vkguide](https://vkguide.dev/)

