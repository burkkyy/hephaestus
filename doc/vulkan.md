# vulkan notes

## Vulkan functionality levels

### Instance level

- Commands that enumerate instance properties
- Commands that dispatch from vk::PhysicalDevice(or vk::PhysicalDevice as a parameter) if the functionality is specified by an instance extension

### Device level

- Commands that dispatch from a vk::Device object or a child object of a vk::Device, or take any of them as a parameter
- Types defined by a device extension
- Commands that enumerate physical device properties are considered device-level functionality
- Commands that dispatch from vk::PhysicalDevice(or vk::PhysicalDevice as a parameter) if the functionality is specified by a device extension
