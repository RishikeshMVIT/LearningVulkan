#pragma once

#include "Defines.h"

class Application
{
public:
	void Initialize();
	void Run();
	void Shutdown();

private:
	void InitializeWindow();
	void InitializeVulkan();

	//void SetupDebugMessenger();
	void CreateVulkanInstance();
	void PickPhysicalDevice();
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
	//

	std::vector<const char*> GetRequiredExtensions();
	std::vector<const char*> GetRequiredLayers();
	bool IsPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);

	void MessageLoop();

	void CleanUp();

#ifdef _DEBUG
	VkResult CheckValidationLayerSupport();
#endif // DEBUG


private:
	bool m_isRunning = false;

	GLFWwindow* m_window;
	VkInstance	m_instance;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

#ifdef _DEBUG
	VkDebugUtilsMessengerEXT m_debugMessenger;
#endif // _DEBUG

private:
#ifdef _DEBUG
	static const std::vector<const char*> k_validationLayers;
#endif // _DEBUG

};