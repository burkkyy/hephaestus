#include "device.hpp"

#include <string.h>

#include <set>

#include "util/logger.hpp"

namespace hep {

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT m_severity,
              VkDebugUtilsMessageTypeFlagsEXT m_type,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallback_data,
              void* pUser_data) {
  (void)m_type;
  (void)pCallback_data;
  (void)pUser_data;

  if (m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    log::error(pCallback_data->pMessage);
    return VK_SUCCESS;
  } else if (m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    log::warning(pCallback_data->pMessage);
  } else {
    log::verbose(pCallback_data->pMessage);
  }

  return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pCallback) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pCallback);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT callback,
                                   const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) { func(instance, callback, pAllocator); }
}

Device::Device(Window& window) : window{window} {
  createVulkanInstance();
  setupDebugMessenger();
  this->window.createSurface(*instance, surface);
  pickPhysicalDevice();
  createLogicalDevice();
  createCommandPool();
}

Device::~Device() {
  if (this->enableValidationLayers) {
    destroyDebugUtilsMessengerEXT(this->instance.get(), this->debugMessenger,
                                  nullptr);
    log::verbose("destroyed Vulkan Debugger");
  }

  this->device->destroyCommandPool(commandPool);
  log::verbose("destroyed vk::CommandPool");

  this->instance->destroySurfaceKHR(surface);
  log::verbose("destroyed vk::SurfaceKHR");
}

u32 Device::findMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties) {
  vk::PhysicalDeviceMemoryProperties memoryProperties =
      this->physicalDevice.getMemoryProperties();

  for (u32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memoryProperties.memoryTypes[i].propertyFlags & properties) ==
            properties) {
      return i;
    }
  }

  log::fatal("failed to find suitable memory type");
  throw std::runtime_error("failed to find suitable memory type");
}

vk::Format Device::findSupportedFormat(
    const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
    vk::FormatFeatureFlags features) {
  for (vk::Format format : candidates) {
    vk::FormatProperties properties =
        physicalDevice.getFormatProperties(format);

    if ((tiling == vk::ImageTiling::eLinear &&
         (properties.linearTilingFeatures & features) == features) ||
        (tiling == vk::ImageTiling::eOptimal &&
         (properties.optimalTilingFeatures & features) == features)) {
      return format;
    }
  }

  log::fatal("failed to find supported format");
  throw std::runtime_error("failed to find supported format");
}

void Device::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                          vk::MemoryPropertyFlags properties,
                          vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
  vk::BufferCreateInfo bufferInfo = {};
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = vk::SharingMode::eExclusive;

  try {
    buffer = this->device->createBuffer(bufferInfo);
  } catch (const vk::SystemError& error) {
    log::fatal("failed to create vertex buffer");
    throw std::runtime_error("failed to create vertex buffer");
  }

  vk::MemoryRequirements memoryRequirements =
      this->device->getBufferMemoryRequirements(buffer);

  vk::MemoryAllocateInfo allocInfo = {};
  allocInfo.allocationSize = memoryRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memoryRequirements.memoryTypeBits, properties);

  try {
    bufferMemory = this->device->allocateMemory(allocInfo);
  } catch (const vk::SystemError& error) {
    log::fatal("failed to allocate vertex buffer memory");
    throw std::runtime_error("failed to allocate vertex buffer memory");
  }

  this->device->bindBufferMemory(buffer, bufferMemory, 0);
}

void Device::createImageWithInfo(const vk::ImageCreateInfo& imageInfo,
                                 vk::MemoryPropertyFlags properties,
                                 vk::Image& image,
                                 vk::DeviceMemory& imageMemory) {
  try {
    vk::Result result = this->device->createImage(&imageInfo, nullptr, &image);
    if (result != vk::Result::eSuccess) { throw vk::SystemError(result); }
  } catch (const vk::SystemError& error) {
    log::fatal("failed to create image. Error: ", error.what());
    throw std::runtime_error("failed to create image");
  }

  vk::MemoryRequirements memoryRequirements =
      this->device->getImageMemoryRequirements(image);

  vk::MemoryAllocateInfo allocateInfo{};
  allocateInfo.allocationSize = memoryRequirements.size;
  allocateInfo.memoryTypeIndex =
      findMemoryType(memoryRequirements.memoryTypeBits, properties);

  try {
    vk::Result result =
        this->device->allocateMemory(&allocateInfo, nullptr, &imageMemory);
    if (result != vk::Result::eSuccess) { throw vk::SystemError(result); }
  } catch (const vk::SystemError& error) {
    log::fatal("failed to allocate image memory. Error: ", error.what());
    throw std::runtime_error("failed to allocate image memory");
  }

  try {
    this->device->bindImageMemory(image, imageMemory, 0);
  } catch (const vk::SystemError& error) {
    log::fatal("failed to bind image memory. Error: ", error.what());
    throw std::runtime_error("failed to bind image memory");
  }
}

void Device::setupDebugMessenger() {
  if (!this->enableValidationLayers) return;

  auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
      vk::DebugUtilsMessengerCreateFlagsEXT(),
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      debugCallback, nullptr);

  if (createDebugUtilsMessengerEXT(
          *instance,
          reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(
              &createInfo),
          nullptr, &this->debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug callback!");
  }
}

bool Device::checkValidationLayerSupport() {
  auto availableLayers = vk::enumerateInstanceLayerProperties();
  for (const char* layerName : enabledLayers) {
    bool layerFound = false;

    for (const auto& layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) { return false; }
  }

  return true;
}

std::vector<const char*> Device::getRequiredExtensions() {
  u32 glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char*> extensions(glfw_extensions,
                                      glfw_extensions + glfw_extension_count);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

#ifndef NDEBUG
  u32 available_extension_count = 0;
  vk::Result result = vk::enumerateInstanceExtensionProperties(
      nullptr, &available_extension_count, nullptr);

  if (result != vk::Result::eSuccess) { throw std::exception(); }

  std::vector<vk::ExtensionProperties> available_extensions(
      available_extension_count);

  result = vk::enumerateInstanceExtensionProperties(
      nullptr, &available_extension_count, available_extensions.data());

  if (result != vk::Result::eSuccess) { throw std::exception(); }

  log::verbose("Number of available extensions: ", available_extension_count);

  log::verbose("Available extensions:");
  for (const auto& e : available_extensions) {
    std::cout << '\t' << e.extensionName << '\n';
  }
  std::cout << "Required extensions:\n";
  for (const auto& e : extensions) { std::cout << "\t" << e << '\n'; }
#endif

  return extensions;
}

void Device::createVulkanInstance() {
  if (this->enableValidationLayers && !checkValidationLayerSupport()) {
    log::fatal("Validation layers requested but not available.");
    throw std::exception();
  }

  vk::ApplicationInfo app_info = vk::ApplicationInfo(
      "Hephaestus", VK_MAKE_VERSION(1, 0, 0), "Hephaestus Vulkan Engine",
      VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_3);

  auto extensions = getRequiredExtensions();

  auto createInfo = vk::InstanceCreateInfo(
      vk::InstanceCreateFlags(), &app_info, 0, nullptr,
      static_cast<uint32_t>(extensions.size()), extensions.data());

  createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
  createInfo.ppEnabledLayerNames = enabledLayers.data();

  try {
    this->instance = vk::createInstanceUnique(createInfo, nullptr);
  } catch (const vk::SystemError& err) {
    throw std::runtime_error("Failed to create instance!");
  }
}

QueueFamilyIndices Device::findQueueFamilies(vk::PhysicalDevice device) {
  QueueFamilyIndices indices;
  auto queueFamilies = device.getQueueFamilyProperties();
  int i = 0;

  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 &&
        queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
      indices.graphicsFamily = i;
    }

    if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, surface)) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) { break; }
    i++;
  }

  return indices;
}

bool Device::checkDeviceExtensionSupport(const vk::PhysicalDevice& device) {
  std::set<std::string> requiredExtensions(this->enabledExtensions.begin(),
                                           this->enabledExtensions.end());

  for (const auto& extension : device.enumerateDeviceExtensionProperties()) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

bool Device::isPhysicalDeviceSuitable(const vk::PhysicalDevice& device) {
  QueueFamilyIndices indices = findQueueFamilies(device);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapchainAdequate = false;
  if (extensionsSupported) {
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device);
    swapchainAdequate = !swapchainSupport.formats.empty() &&
                        !swapchainSupport.presentModes.empty();
  }
  return indices.isComplete() && extensionsSupported && swapchainAdequate;
}

void Device::pickPhysicalDevice() {
  // Get all physical devies
  std::vector<vk::PhysicalDevice> physicalDevices =
      instance->enumeratePhysicalDevices();

  if (physicalDevices.empty()) {
    log::fatal("Failed to find any physical devices.");
    throw std::exception();
  }
  log::verbose("Physical Devices count: ", physicalDevices.size());

  // Select first physical device that is suitable
  for (const auto& device : physicalDevices) {
    if (isPhysicalDeviceSuitable(device)) {
      this->physicalDevice = device;
      break;
    }
  }

  if (this->physicalDevice == VK_NULL_HANDLE) {
    log::fatal("Failed to find a suitable physical device");
    throw std::exception();
  }

  vk::PhysicalDeviceProperties properties =
      this->physicalDevice.getProperties();
  log::verbose("Physical Device: ", properties.deviceName);
}

void Device::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(this->physicalDevice);

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePriority = 1.0f;

  for (uint32_t queueFamily : uniqueQueueFamilies) {
    queueCreateInfos.push_back(
        {vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority});
  }

  auto deviceFeatures = vk::PhysicalDeviceFeatures();
  auto createInfo = vk::DeviceCreateInfo(
      vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()),
      queueCreateInfos.data());
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(this->enabledExtensions.size());
  createInfo.ppEnabledExtensionNames = this->enabledExtensions.data();

  if (this->enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(this->enabledLayers.size());
    createInfo.ppEnabledLayerNames = this->enabledLayers.data();
  }

  try {
    this->device = this->physicalDevice.createDeviceUnique(createInfo);
    log::verbose("created vk::Device.");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create logical device.");
    throw std::exception();
  }

  this->graphicsQueue = device->getQueue(indices.graphicsFamily.value(), 0);
  this->presentQueue = device->getQueue(indices.presentFamily.value(), 0);
}

SwapchainSupportDetails Device::querySwapchainSupport(
    vk::PhysicalDevice device) {
  SwapchainSupportDetails details;
  details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
  details.formats = device.getSurfaceFormatsKHR(surface);
  details.presentModes = device.getSurfacePresentModesKHR(surface);

  return details;
}

void Device::createCommandPool() {
  QueueFamilyIndices queueFamilyIndices =
      findQueueFamilies(this->physicalDevice);

  vk::CommandPoolCreateInfo createInfo = {};
  createInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  createInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  try {
    this->commandPool = this->device->createCommandPool(createInfo);
    log::verbose("created vk::CommandPool");
  } catch (const vk::SystemError& err) {
    log::fatal("failed to create vk::CommandPool");
    throw std::runtime_error("failed to create vk::CommandPool");
  }
}

}  // namespace hep
