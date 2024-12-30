#include "device.hpp"

#include "util/logger.hpp"
#include <string.h>

namespace hep {

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
  VkDebugUtilsMessageSeverityFlagBitsEXT m_severity,
  VkDebugUtilsMessageTypeFlagsEXT m_type,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallback_data,
  void* pUser_data
){
  (void)m_type;
  (void)pCallback_data;
  (void)pUser_data;

  if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT){
    log::error(pCallback_data->pMessage);
    return VK_SUCCESS;  // Abort program
  } else if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
    log::warning(pCallback_data->pMessage);
  } else {
    log::verbose(pCallback_data->pMessage);
  }

  return VK_FALSE;
}

Device::Device(Window& window) : window{window} {
  create_vulkan_instance();
	setup_debug_messenger();
	// window.create_surface(instance, &surface);
	// pick_physical_device();
	// create_logical_device();
}

Device::~Device(){
	if(enable_validation_layers){
		destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
    log::info("Destroyed Vulkan Debugger.");
	}

  vkDestroyInstance(instance, nullptr);
  log::info("Destroyed VkInstance.");
}

void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info){
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debug_callback;
}

void Device::setup_debug_messenger(){
  if(!enable_validation_layers){ return; }

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  populate_debug_messenger_create_info(create_info);

  VkResult result = create_debug_utils_messenger_EXT(instance, &create_info, nullptr, &debug_messenger);
  if(result != VK_SUCCESS) {
    log::fatal("Failed to set up debug messenger. Error code: ", result);
    throw std::exception();
  }
  log::info("Set up debug messenger.");
}

VkResult Device::create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if(func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void Device::destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if(func != nullptr){ func(instance, debugMessenger, pAllocator); }
}

bool Device::check_validation_layer_support(){
	u32 layer_count = 0;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

	// Check if properties of each layer is available
	for(auto enabled_layer : enabled_layers){
		bool layer_found = false;

		for(const auto& layer : layers){
			if(strcmp(enabled_layer, layer.layerName) == 0){
				layer_found = true;
				break;
			}
		}
		if(!layer_found){ return false; }
	}
	return true;
}

std::vector<const char*> Device::get_required_extensions(){
	u32 glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

  if(enable_validation_layers){
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  u32 available_extension_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, nullptr); // get # of extentions
  std::vector<VkExtensionProperties> available_extensions(available_extension_count); // set up vector to hold VkExtensionProperties structs
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, available_extensions.data());   // get VkExtensionProperties structs

  log::verbose("Number of available extensions: ", available_extension_count);

#ifndef NDEBUG
  log::verbose("Available extensions:");
  for(const auto& e : available_extensions){ std::cout << '\t' << e.extensionName << '\n'; }
  std::cout << "Required extensions:\n";
  for(const auto& e : extensions){ std::cout << "\t" << e << '\n'; }
#endif
  return extensions;
}

void Device::create_vulkan_instance(){
	if(enable_validation_layers && !check_validation_layer_support()){
    log::fatal("Validation layers requested but not available.");
		throw std::exception();
	}

	VkApplicationInfo application_info = {};
  application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  application_info.pApplicationName = "Hephaestus";
  application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  application_info.pEngineName = "Hephaestus Vulkan Engine";
  application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  application_info.apiVersion = VK_API_VERSION_1_3;
	
	auto extensions = get_required_extensions();

	/*
	 * Tell vulkan which extensions and validation layers we want to use.
	 * These extensions are global.
	 */
	VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &application_info;
  create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();
  create_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
  create_info.ppEnabledLayerNames = enabled_layers.data();

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
  if(enable_validation_layers){
    log::info("Vulkan validation layers are enabled.");
    populate_debug_messenger_create_info(debug_create_info);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
  }

  VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
  if(result != VK_SUCCESS){
    log::fatal("Failed to create vulkan instance.");
    throw std::exception();
  }

  log::info("Created VkInstance.");
}

} // namespace hep
