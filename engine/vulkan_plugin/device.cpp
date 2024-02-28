/**
 * @file engine/vulkan/device.hpp
 * @author Caleb Burke
 * @date Jan 14, 2024
 */

#include "device.hpp"

#include "window.hpp"
#include "util/util.hpp"

#include <set>
#include <vulkan/vulkan.h>

namespace hep {
namespace vul {

/**
 * @brief Vulkan debug callback function for handling validation layer messages.
 *
 * This function is intended to be used as a callback for Vulkan debug messages generated
 * by the validation layers. It logs the messages with different severity levels.
 *
 * @param m_severity The severity level of the debug message.
 * @param m_type The type of the debug message.
 * @param pCallback_data Pointer to a structure containing debug information.
 * @param pUser_data A pointer to user-defined data, which can be set when registering the callback.
 * @return VK_FALSE to indicate that the Vulkan call that triggered the validation message
 *         should not be aborted, allowing the program to continue execution.
 */
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT m_severity,
    VkDebugUtilsMessageTypeFlagsEXT m_type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallback_data,
    void* pUser_data){
    (void)m_type;
    (void)pCallback_data;
    (void)pUser_data;
    if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT){
        log(LEVEL::ERROR, pCallback_data->pMessage);
        return VK_SUCCESS;  // Abort program
    } else if(m_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
        log(LEVEL::WARNING, pCallback_data->pMessage);
    } else {
        log(LEVEL::TRACE, pCallback_data->pMessage);
    }
    return VK_FALSE;
}

/**
 * @brief Constructor
 * @note Only constructor
 * @param[in] window
 */
Device::Device(Window& window) : window{window} {
	initialize();
}
	
/**
 * @brief Default Deconstructor
 * 
 * Cleans up:
 *  1. Vulkan Debugger
 *  2. Command Buffers
 *  3. Logical Device / Queues
 *  4. Vulkan Instance
 *
 * TODO Destroy command buffers
 */
Device::~Device(){
	if(enable_validation_layers){
		destroy_debug_utils_messenger_EXT(instance, debug_messenger, nullptr);
		log(LEVEL::TRACE, "Destroyed Vulkan Debugger.");
	}

    vkDestroyDevice(device, nullptr);
    log(LEVEL::TRACE, "Destroyed Logical Device.");

	vkDestroySurfaceKHR(instance, surface, nullptr);
   	log(LEVEL::TRACE, "Destroyed VkSurfaceKHR.");

    // Destroy vulkan instance last
    vkDestroyInstance(instance, nullptr);
    log(LEVEL::TRACE, "Destroyed VkInstance.");
}

/**
 * @brief Initializes device
 *
 * TODO create logical device
 * TODO create command pool
 *
 * @note Called by constructor
 * @return void
 */
void Device::initialize(){
	create_vulkan_instance();
	setup_debug_messenger();
	window.create_surface(instance, &surface);
	pick_physical_device();
	create_logical_device();
}
	
/**
 * @brief Creates vulkan instance
 * @return void
 */
void Device::create_vulkan_instance(){
	if(enable_validation_layers && !check_validation_layer_support()){
		log(LEVEL::ERROR, "Validation layers requested but not available.");
		throw std::exception();
	}

	// Tell driver which vulkan api version we are using
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
       	log(LEVEL::INFO, "Vulkan validation layers are enabled.");
       	populate_debug_messenger_create_info(debug_create_info);
       	create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    }	

    VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
    if(result != VK_SUCCESS){
       	log(LEVEL::FATAL, "Failed to create vulkan instance.");
    	throw std::exception();
	}
	log(LEVEL::TRACE, "Created VkInstance.");
}

/**
 * @brief Checks if the layers we enabled are available
 * @return If layers are available
 */
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
	
/**
 * @brief Gets extentions needed
 * @return list of extensions we need enabled
 */
std::vector<const char*> Device::get_required_extensions(){
	/**
	 * GLFW Extensions
     * Extensions for vulkan to interface with GLFW so we use built in 
     * glfw function that returns the extension(s)
	 */
	u32 glfw_extension_count = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	
	// vector to hold all extensions
    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if(enable_validation_layers){
       	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    u32 available_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, nullptr); // get # of extentions
    std::vector<VkExtensionProperties> available_extensions(available_extension_count); // set up vector to hold VkExtensionProperties structs
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, available_extensions.data());   // get VkExtensionProperties structs

    log(LEVEL::TRACE, "Number of available extensions: ", available_extension_count);
    
#ifndef NDEBUG
    log(LEVEL::TRACE, "Available extensions:");
    for(const auto& e : available_extensions){ std::cout << '\t' << e.extensionName << '\n'; }
    std::cout << "Required extensions:\n";
    for(const auto& e : extensions){ std::cout << "\t" << e << '\n'; }
#endif
    return extensions;
}

/**
 * @brief Creates debugger info
 *
 * Sets up the info struct for creating the debugger
 *
 * @param[out] create_info
 * @return void
 */
void Device::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info){
   	create_info = {};
   	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
   	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
   	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
   	create_info.pfnUserCallback = debug_callback;
}
    
/**
 * @brief Create Debugger
 * @return void
 */
void Device::setup_debug_messenger(){
   	if(!enable_validation_layers){ return; }

   	VkDebugUtilsMessengerCreateInfoEXT create_info;
   	populate_debug_messenger_create_info(create_info);

   	VkResult result = create_debug_utils_messenger_EXT(instance, &create_info, nullptr, &debug_messenger);
   	if(result != VK_SUCCESS){
   	    log(LEVEL::FATAL, "Failed to set up debug messenger. Error code: ", result);
		throw std::exception();
   	}
   	log(LEVEL::TRACE, "Set up debug messenger.");
}

/**
 * @brief 
 *
 * Not sure what this does but its required for vulkan debugger
 *
 * @param instance
 * @param pCreateInfo
 * @param pAllocator
 * @param pDebugMessenger
 * @return
 */
VkResult Device::create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
   	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
   	if(func != nullptr) {
       	return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
   	} else {
       	return VK_ERROR_EXTENSION_NOT_PRESENT;
   	}
}

/**
 * @brief
 * @param instance
 * @param debugMessenger
 * @param pAllocator
 * @return void
 */
void Device::destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
   	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
   	if(func != nullptr){ func(instance, debugMessenger, pAllocator); }
}
	
/**
 * @brief Picks physical device to use
 * 
 * Picks the physical device vulkan will use to render with
 *
 * @return void
 */
void Device::pick_physical_device(){
	// Query number of physical devices
	u32 count = 0;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);

	if(count <= 0){
		log(LEVEL::FATAL, "Failed to find any physical devices.");
		throw std::exception();
	}
	log(LEVEL::TRACE, "Physical Devices count: ", count);

   	// Get all physical devies
   	std::vector<VkPhysicalDevice> physical_devices(count);
   	vkEnumeratePhysicalDevices(instance, &count, physical_devices.data());

   	// Select first physical device that is suitable
   	for(const auto& device : physical_devices){
       	if(is_physical_device_suitable(device)){
           	physical_device = device;
           	break;
       	}
	}

   	if(physical_device == VK_NULL_HANDLE){
       	log(LEVEL::FATAL, "Failed to find a suitable physical device");
       	throw std::exception();
   	}

   	VkPhysicalDeviceProperties properties;
   	vkGetPhysicalDeviceProperties(physical_device, &properties);
   	log(LEVEL::TRACE, "Physical Device: ", properties.deviceName);
}

/**
 * @brief Checks if device is suitable
 *
 * Checks if a physical device is suitable to use for our program.
 *
 * @param[in] physical_device The device we are checking
 * @return If device is suitable
 */
bool Device::is_physical_device_suitable(VkPhysicalDevice physical_device){
   	QueueFamilyIndices indices = find_queue_families(physical_device);
   	bool extensions_support = check_device_extension_support(physical_device);
    log(LEVEL::DEBUG, "Indices: ", indices.graphics.value(), " ", indices.present.value());
   	
   	VkPhysicalDeviceProperties properties;
   	vkGetPhysicalDeviceProperties(physical_device, &properties);
   	
    VkPhysicalDeviceFeatures features;
   	features.samplerAnisotropy = VK_TRUE;
    vkGetPhysicalDeviceFeatures(physical_device, &features);
   	
   	bool support_swapchain = false;
   	if(extensions_support){
       	SwapChainSupportDetails support = query_swapchain_support(physical_device);
       	support_swapchain = !support.formats.empty() && !support.present_modes.empty();
   	}

	return indices.filled() && extensions_support && support_swapchain;
}
	
/**
 * @brief Find the queue families of a physical device
 *
 * Find the queue families of a physical device. 
 *
 * @param[in] physical_device The physical device we search in
 * @return void
 */
QueueFamilyIndices Device::find_queue_families(VkPhysicalDevice physical_device){
	QueueFamilyIndices indices;
    
   	// Get number of queue families available in device
   	u32 count = 0;
   	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);
    log(LEVEL::DEBUG, "Physical device queue familiy count: ", count);

   	// Get queue family properties of device
   	std::vector<VkQueueFamilyProperties> queue_families(count);
   	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, queue_families.data());

   	for(size_t i = 0; i < count; i++){
       	if(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
           	indices.graphics = i;
       	}

       	VkBool32 present_support = false;
       	vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
       	if(present_support){
           	indices.present = i;
       	}
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
bool Device::check_device_extension_support(VkPhysicalDevice physical_device){
   	uint32_t count;
   	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, nullptr);

   	std::vector<VkExtensionProperties> extensions(count);
   	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, extensions.data());

   	std::set<std::string> required_extensions(enabled_extensions.begin(), enabled_extensions.end());

   	for(const auto& extension : extensions){
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
SwapChainSupportDetails Device::query_swapchain_support(VkPhysicalDevice physical_device){
   	SwapChainSupportDetails details;
   	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);
   
   	u32 format_count, present_count;
   	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
   	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_count, nullptr);

	if(format_count != 0){
       	details.formats.resize(format_count);
       	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details.formats.data());
    }

    if(present_count != 0){
       	details.present_modes.resize(present_count);
       	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_count, details.present_modes.data());
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
void Device::create_logical_device(){
	QueueFamilyIndices indices = find_queue_families(physical_device);

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<u32> unique_queue_families = {
        indices.graphics.value(),
        indices.present.value()
    };
	f32 queue_priority = 1.0f;

    for(u32 queue : unique_queue_families){
        log(LEVEL::TRACE, "Creating Queue Family: ", queue);
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
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
    create_info.ppEnabledExtensionNames = enabled_extensions.data();

    if(enable_validation_layers){
        create_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
        create_info.ppEnabledLayerNames = enabled_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }
    
    VkResult result = vkCreateDevice(physical_device, &create_info, nullptr, &device);
    if(result != VK_SUCCESS){
       	log(LEVEL::FATAL, "Failed to create logical device.");
    	throw std::exception();
    }

    vkGetDeviceQueue(device, indices.graphics.value(), 0, &graphics_queue);
    vkGetDeviceQueue(device, indices.present.value(), 0, &present_queue);
    log(LEVEL::TRACE, "Created Logical Device.");
    log(LEVEL::TRACE, "Created Graphics Queue.");
    log(LEVEL::TRACE, "Created Present Queue.");
}

}	// namespace vul
}   // namespace hep

