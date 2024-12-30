#include "device.hpp"

#include <string.h>

#include "util/logger.hpp"

namespace hep {

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT m_severity,
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
  window.createSurface(*instance, surface);
  pickPhysicalDevice();
  // create_logical_device();
}

Device::~Device() {
  if (enable_validation_layers) {
    destroyDebugUtilsMessengerEXT(instance.get(), debug_messenger, nullptr);
    log::verbose("Destroyed Vulkan Debugger.");
  }

  // vkDestroyDevice(device, nullptr);
  // log::verbose("Destroyed Logical Device.");

  vkDestroySurfaceKHR(instance.get(), surface, nullptr);
  log::verbose("Destroyed VkSurfaceKHR.");
}

void Device::setupDebugMessenger() {
  if (!enable_validation_layers) return;

  auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
      vk::DebugUtilsMessengerCreateFlagsEXT(),
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      debug_callback, nullptr);

  if (createDebugUtilsMessengerEXT(
          *instance,
          reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(
              &createInfo),
          nullptr, &debug_messenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug callback!");
  }
}

bool Device::checkValidationLayerSupport() {
  auto available_layers = vk::enumerateInstanceLayerProperties();
  for (const char* layer_name : enabled_layers) {
    bool layer_found = false;

    for (const auto& layer_properties : available_layers) {
      if (strcmp(layer_name, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) { return false; }
  }

  return true;
}

std::vector<const char*> Device::getRequiredExtensions() {
  u32 glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char*> extensions(glfw_extensions,
                                      glfw_extensions + glfw_extension_count);

  if (enable_validation_layers) {
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
  if (enable_validation_layers && !checkValidationLayerSupport()) {
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

  createInfo.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
  createInfo.ppEnabledLayerNames = enabled_layers.data();

  try {
    instance = vk::createInstanceUnique(createInfo, nullptr);
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
    if (indices.isComplete()) { break; }
    i++;
  }
  return indices;
}

bool Device::isPhysicalDeviceSuitable(const vk::PhysicalDevice& device) {
  QueueFamilyIndices indices = findQueueFamilies(device);
  return indices.isComplete();
}

void Device::pickPhysicalDevice() {
  // Get all physical devies
  std::vector<vk::PhysicalDevice> physical_devices =
      instance->enumeratePhysicalDevices();

  if (physical_devices.empty()) {
    log::fatal("Failed to find any physical devices.");
    throw std::exception();
  }
  log::verbose("Physical Devices count: ", physical_devices.size());

  // Select first physical device that is suitable
  for (const auto& device : physical_devices) {
    if (isPhysicalDeviceSuitable(device)) {
      physical_device = device;
      break;
    }
  }

  if (physical_device == VK_NULL_HANDLE) {
    log::fatal("Failed to find a suitable physical device");
    throw std::exception();
  }

  vk::PhysicalDeviceProperties properties = physical_device.getProperties();
  log::verbose("Physical Device: ", properties.deviceName);
}

}  // namespace hep
