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
    CreateVulkanSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapchain();
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

std::vector<const char*> Application::GetRequiredDeviceExtensions()
{
    std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    return requiredExtensions;
}

bool Application::IsPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
{
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

    B32 extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

    B32 isSwapchainAdequate = false;
    if (extensionsSupported)
    {
        SwapchainSupportDetails swapchainSupportDetails = QuerySwapchainSupport(physicalDevice);
        isSwapchainAdequate = !swapchainSupportDetails.formats.empty() && !swapchainSupportDetails.presentModes.empty();
    }

    return indices.IsComplete() && extensionsSupported && isSwapchainAdequate;
}

bool Application::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    auto deviceExtensions = GetRequiredDeviceExtensions();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) 
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
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

void Application::CreateVulkanSurface()
{
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    //surfaceInfo.pNext     = nullptr;
    //surfaceInfo.flags;
    surfaceInfo.hinstance   = GetModuleHandle(nullptr);
    surfaceInfo.hwnd        = glfwGetWin32Window(m_window);

    VK_CHECK(vkCreateWin32SurfaceKHR(m_instance, &surfaceInfo, nullptr, &m_surface), "Failed to create Vulkan Surface");
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

    VK_CHECK(m_physicalDevice == VK_NULL_HANDLE, "Failed to find a suitable GPU.");

}

void Application::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
    F32 queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    std::set<U32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    for (U32 queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        //queueInfo.pNext             = nullptr;
        //queueInfo.flags             = nullptr;
        queueInfo.queueFamilyIndex = queueFamily;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;
        queueInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
    auto layers = GetRequiredLayers();
    auto deviceExtensions = GetRequiredDeviceExtensions();

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    //deviceInfo.pNext                    = nullptr;
    //deviceInfo.flags                    = nullptr;
    deviceInfo.queueCreateInfoCount     = static_cast<U32>(queueInfos.size());
    deviceInfo.pQueueCreateInfos        = queueInfos.data();
    deviceInfo.enabledLayerCount        = static_cast<U32>(layers.size());
    deviceInfo.ppEnabledLayerNames      = layers.data();
    deviceInfo.enabledExtensionCount    = static_cast<U32>(deviceExtensions.size());
    deviceInfo.ppEnabledExtensionNames  = deviceExtensions.data();
    deviceInfo.pEnabledFeatures         = &physicalDeviceFeatures;

    VK_CHECK(vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device), "Failed to create a Vulkan Logical Device.");

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);

}

void Application::CreateSwapchain()
{
    SwapchainSupportDetails swapchainSupportDetails = QuerySwapchainSupport(m_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = PickSwapchainFormat(swapchainSupportDetails.formats);
    VkPresentModeKHR presentMode = PickSwapchainPresentMode(swapchainSupportDetails.presentModes);
    VkExtent2D extent = PickSwapchainExtent(swapchainSupportDetails.capabilities);

    U32 imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;

    if (swapchainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapchainSupportDetails.capabilities.maxImageCount)
    {
        imageCount = swapchainSupportDetails.capabilities.maxImageCount;
    }
    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
    U32 queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType                     = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    //swapchainInfo.pNext;
    //swapchainInfo.flags;
    swapchainInfo.surface                   = m_surface;
    swapchainInfo.minImageCount             = imageCount;
    swapchainInfo.imageFormat               = surfaceFormat.format;
    swapchainInfo.imageColorSpace           = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent               = extent;
    swapchainInfo.imageArrayLayers          = 1;
    swapchainInfo.imageUsage                = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (indices.graphicsFamily != indices.presentFamily) 
    {
        swapchainInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else 
    {
        swapchainInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 0; // Optional
        swapchainInfo.pQueueFamilyIndices   = nullptr; // Optional
    }

    swapchainInfo.preTransform              = swapchainSupportDetails.capabilities.currentTransform;
    swapchainInfo.compositeAlpha            = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode               = presentMode;
    swapchainInfo.clipped                   = VK_TRUE;
    swapchainInfo.oldSwapchain              = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(m_device, &swapchainInfo, nullptr, &m_swapchain), "Failed to create Vulkan Swapchain")

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
    m_swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data());
    m_swapchainFormat = surfaceFormat.format;
    m_swapchainExtent = extent;

}

VkSurfaceFormatKHR Application::PickSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Application::PickSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) 
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Application::PickSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<U32>::max())) 
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
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

        VkBool32 presentSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_surface, &presentSupported);

        if (presentSupported)
        {
            indices.presentFamily = i;
        }

        if (indices.IsComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

SwapchainSupportDetails Application::QuerySwapchainSupport(VkPhysicalDevice physicalDevice)
{
    SwapchainSupportDetails details = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &details.capabilities);
    
    U32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &formatCount, details.formats.data());
    }

    U32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
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
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    vkDestroyDevice(m_device, nullptr);

#ifdef _DEBUG
	DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
#endif // _DEBUG

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

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
