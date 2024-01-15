# hephaestus

## Build Instructions
```bash
./setup.sh
make
```
Engine found in `engine/build/lib`. To use the shared library, env variable `LD_LIBRARY_PATH` must be set so dynamic linker can find it.

```bash
source ./env.sh
```

## Dependencies
- `base-devel`
- `cmake`
- `vulkan-devel`

