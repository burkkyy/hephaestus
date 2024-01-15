# Vulkan

## Vulkan Objects
Using vulkan you will be creating many different kinds of objects. There are many different kinds of vulkan objects, with many different levels of abstraction between them. See *figure 1* for a diagram of the different levels of abstraction.

![vulkan-abstraction](/media/vulkan_abstraction.svg)
*Figure 1. Vulkan Abstraction*

Here I will be briefly going over the main objects.

### Vulkan Instance
The Vulkan instanace(`VkInstance`) is a handle to the Vulkan installation. The Vulkan instance stores information about your application to the Vulkan driver.

### Physical Device
The Vulkan physical device(`VkPhysicalDevice`) is a handle to a single hardware device on the host system(GPU) that has Vulkan drivers. Only one physical device handle is created per hardware device.

### Logical Device
A Vulkan logical device(`VkDevice`) is the main interface between your application and the physical device. Logical devices define which queues and extensions are in use. You can have multiple logical devices per physical device.

### Queues
A Vulkan Queue(`VkQueue`) is an object that stores a bunch of commands to be done on the hardware. A queue receives these commands in command buffers. These command buffers are then queued for processing, and then are processed by the physical device. A queue belongs to a `queue family`. To use a different `queue family`, like `compute queues`, these must be checked they are available in the physical device.

### Command Pool and Command Buffers

## Swapchain

