
QueueFamilyIndices Device::find_queue_families(
    VkPhysicalDevice physical_device) {
  QueueFamilyIndices indices;

  // Get number of queue families available in device
  u32 count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);
  log::debug("Physical device queue familiy count: ", count);

  // Get queue family properties of device
  std::vector<VkQueueFamilyProperties> queue_families(count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count,
                                           queue_families.data());

  for (size_t i = 0; i < count; i++) {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = i;
    }

    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface,
                                         &present_support);
    if (present_support) { indices.present = i; }
  }

  return indices;
}

/**
 * @brief Check if physical device supports extensions
 *
 * Checks if a physical device supports vulkan extensions for this program
 *
 * @param[in] physical_device The physical device we search in
 * @return If physical device supports extensions
 */
bool Device::check_device_extension_support(VkPhysicalDevice physical_device) {
  uint32_t count;
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count,
                                       nullptr);

  std::vector<VkExtensionProperties> extensions(count);
  vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count,
                                       extensions.data());

  std::set<std::string> required_extensions(enabled_extensions.begin(),
                                            enabled_extensions.end());

  for (const auto& extension : extensions) {
    required_extensions.erase(extension.extensionName);
  }
  return required_extensions.empty();
}

/**
 * @brief Get details on physical device
 *
 * Get details on physical device, details that can be used to determine
 * if a physical device supports swapchain
 *
 * @param[in] physical_device The physical device we search in
 * @return The support details of physical device
 */
SwapChainSupportDetails Device::query_swapchain_support(
    VkPhysicalDevice physical_device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface,
                                            &details.capabilities);

  u32 format_count, present_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count,
                                       nullptr);
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                            &present_count, nullptr);

  if (format_count != 0) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface,
                                         &format_count, details.formats.data());
  }

  if (present_count != 0) {
    details.present_modes.resize(present_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, surface, &present_count, details.present_modes.data());
  }

  return details;
}

/**
 * @brief Creates the vulkan logical device
 *
 * Creates a logical device with 2 queues. A graphics
 * queue and a present queue. Before calling this function
 * a physical device must have been picked.
 *
 * TODO Enable needed features
 *
 * @note Stores created logical device in `device`
 * @return void
 */
void Device::create_logical_device() {
  QueueFamilyIndices indices = find_queue_families(physical_device);

  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  std::set<u32> unique_queue_families = {indices.graphics.value(),
                                         indices.present.value()};
  f32 queue_priority = 1.0f;

  for (u32 queue : unique_queue_families) {
    log::verbose("Creating Queue Family: ", queue);
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // TODO Enable some features
  VkPhysicalDeviceFeatures features = {};

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount =
      static_cast<uint32_t>(queue_create_infos.size());
  create_info.pQueueCreateInfos = queue_create_infos.data();
  create_info.pEnabledFeatures = &features;
  create_info.enabledExtensionCount =
      static_cast<uint32_t>(enabled_extensions.size());
  create_info.ppEnabledExtensionNames = enabled_extensions.data();

  if (enable_validation_layers) {
    create_info.enabledLayerCount =
        static_cast<uint32_t>(enabled_layers.size());
    create_info.ppEnabledLayerNames = enabled_layers.data();
  } else {
    create_info.enabledLayerCount = 0;
  }

  VkResult result =
      vkCreateDevice(physical_device, &create_info, nullptr, &device);
  if (result != VK_SUCCESS) {
    log::fatal("Failed to create logical device.");
    throw std::exception();
  }

  vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphics_queue);
  vkGetDeviceQueue(device, indices.present.value(), 0, &present_queue);
  log::info("Created Logical Device.");
  log::verbose("Created Graphics Queue.");
  log::verbose("Created Present Queue.");
}

}  // namespace vul
}  // namespace hep
