#include "Application.h"

#ifdef _DEBUG

const std::vector<const char*> Application::k_validationLayers = { "VK_LAYER_KHRONOS_validation" };

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

#endif

void Application::Initialize()
{
	InitializeWindow();
	InitializeVulkan();

	m_isRunning = true;
}

void Application::Run()
{
	MessageLoop();
}

void Application::Shutdown()
{
	CleanUp();
}

void Application::InitializeWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(960, 540, "Vulkan Engine", nullptr, nullptr);
}

void Application::InitializeVulkan()
{
    CreateVulkanInstance();
    PickPhysicalDevice();
}

std::vector<const char*> Application::GetRequiredExtensions()
{
    U32 glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef _DEBUG
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

	return requiredExtensions;
}

std::vector<const char*> Application::GetRequiredLayers()
{
    std::vector<const char*> requiredLayers;

#ifdef _DEBUG
    requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	return requiredLayers;
}

bool Application::IsPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
{
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

    return indices.IsComplete();

    //VkPhysicalDeviceProperties deviceProperties;
    //VkPhysicalDeviceFeatures deviceFeatures;
    //vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    //vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    //return indices.graphicsFamily.has_value() && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
}

void Application::CreateVulkanInstance()
{

#ifdef _DEBUG
    VK_CHECK(CheckValidationLayerSupport(), "Validation Layers requested but unsupported")

        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
    debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    //debugMessengerInfo.pNext			= nullptr;
    //debugMessengerInfo.flags			= nullptr;
    debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerInfo.pfnUserCallback = VkDebugCallback;
    //debugMessengerInfo.pUserData        = nullptr;

#endif

    //App Info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    //appInfo.pNext					= nullptr;
    appInfo.pApplicationName = "Vulkan Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Vulkan Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // Get Required Extensions and Layers
    auto extensions = GetRequiredExtensions();
    auto layers = GetRequiredLayers();

    //Instance Info
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

#ifdef _DEBUG
    instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerInfo;
#else
    instanceInfo.pNext = nullptr;
#endif

    //instanceInfo.flags					= nullptr;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = static_cast<U32>(layers.size());
    instanceInfo.ppEnabledLayerNames = layers.data();
    instanceInfo.enabledExtensionCount = static_cast<U32>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &m_instance), "Failed to create Vulkan Instance.")

    VK_CHECK(CreateDebugUtilsMessengerEXT(m_instance, &debugMessengerInfo, nullptr, &m_debugMessenger), "Failed to create Debug Messenger")
}

void Application::PickPhysicalDevice()
{
    U32 physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

    VK_CHECK(physicalDeviceCount == 0, "No GPUs with Vulkan support found");

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());

    for (const auto& physicalDevice : physicalDevices)
    {
        if (IsPhysicalDeviceSuitable(physicalDevice))
        {
            m_physicalDevice = physicalDevice;
            break;
        }
    }

    VK_CHECK(m_physicalDevice == VK_NULL_HANDLE, "Failed to find a suitable GPU");

}

QueueFamilyIndices Application::FindQueueFamilies(VkPhysicalDevice physicalDevice)
{
    QueueFamilyIndices indices = {};

    U32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    I32 i = 0;
    for (const auto& queueFamilyProperty : queueFamilyProperties)
    {
        if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

void Application::MessageLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
	}
}

void Application::CleanUp()
{
#ifdef _DEBUG
	DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif // _DEBUG

	vkDestroyInstance(m_instance, nullptr);

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

VkResult Application::CheckValidationLayerSupport()
{
    U32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : k_validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }
    }

    return VK_SUCCESS;
}
