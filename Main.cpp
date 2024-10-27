#pragma region Defines

#define GLFW_INCLUDE_VULKAN

#pragma endregion

#pragma region External Includes

#include <GLFW/glfw3.h>

#pragma endregion

#pragma region Standard Includes

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#pragma endregion

#pragma region Macros
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
#pragma endregion

#pragma region Global Constants

const unsigned int WIDTH = 960;
const unsigned int HEIGHT = 540;

#pragma endregion

#pragma region Validation Layer Functions
const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pdebugMessenger )
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != NULL)
	{
		return func(instance, pCreateInfo, pAllocator, pdebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != NULL)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

#pragma endregion

class Application
{
public:
	void Run()
	{
		//Creating and Initializing Window using GLFW
		{
			//Create and Initialize Window
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
		}

		//Checkeing validation Layer Support
		{
			if (enableValidationLayers && !CheckValidationLayerSupport())
			{
				throw std::runtime_error("Validation Layers requested but not supported");

			}
		}

		//Create Vulkan Instance
		{
			//Vulkan Application Info
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = nullptr;
			appInfo.pApplicationName = "Vulkan Application";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Vulkan Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_3;

			//Get Exxtensions and Layers
			auto extensions = GetRequiredExtensions();

			//Vulkan Instance Create Info
			VkInstanceCreateInfo instanceInfo = {};
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			if (enableValidationLayers)
			{
				//Temporary Debug Messenger Info for instance creation
				VkDebugUtilsMessengerCreateInfoEXT tempDebugMessengerInfo{};
				PopulateDebugMessengerCreateInfo(tempDebugMessengerInfo);

				instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&tempDebugMessengerInfo;
				instanceInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
				instanceInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
			{
				instanceInfo.pNext = nullptr;
				instanceInfo.enabledLayerCount = 0;
				instanceInfo.ppEnabledLayerNames = nullptr;
			}
			//instanceInfo.flags					= NULL;
			instanceInfo.pApplicationInfo = &appInfo;
			instanceInfo.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
			instanceInfo.ppEnabledExtensionNames = extensions.data();

			if (vkCreateInstance(&instanceInfo, nullptr, &_instance) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create Vulkan Instance");
			}
		}

		//Create and Initialize Debug Messenger
		{
			if (enableValidationLayers)
			{
				VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
				PopulateDebugMessengerCreateInfo(debugMessengerInfo);

				if (CreateDebugUtilsMessengerEXT(_instance, &debugMessengerInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
				{
					throw std::runtime_error("Faiuled to create Debug Messenger");
				}
			}
		}

		//Main Loop
		{
			while (!glfwWindowShouldClose(_window))
			{
				glfwPollEvents();
			}
		}

		//Cleanup and Shutdown
		{
			if (enableValidationLayers)
			{
				DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
			}

			vkDestroyInstance(_instance, nullptr);

			glfwDestroyWindow(_window);
			glfwTerminate();
		}

	}
private:	
	std::vector<const char*> GetRequiredExtensions()
	{
		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool CheckValidationLayerSupport()
	{
		unsigned int layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}

		return true;
	}

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &pDebugMessengerInfo)
	{
		pDebugMessengerInfo.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		pDebugMessengerInfo.pNext			= nullptr;
		//pDebugMessengerInfo.flags;
		pDebugMessengerInfo.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		pDebugMessengerInfo.messageType		= 
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		pDebugMessengerInfo.pfnUserCallback = DebugCallback;
		//pDebugMessengerInfo.pUserData;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData)
	{
		std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

private:
	GLFWwindow * _window = nullptr;
	VkInstance _instance = nullptr;
	VkDebugUtilsMessengerEXT _debugMessenger = nullptr;
};

int main()
{
	Application app;
	try
	{
		app.Run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
