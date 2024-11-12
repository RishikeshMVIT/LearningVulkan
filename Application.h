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
	void CreateVulkanSurface();		//TODO: Platform Specific : Refactor to platform layer
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapchain();

	VkSurfaceFormatKHR PickSwapchainFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR PickSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D PickSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
	SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice);

	std::vector<const char*> GetRequiredExtensions();
	std::vector<const char*> GetRequiredLayers();
	std::vector<const char*> GetRequiredDeviceExtensions();

	bool IsPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

	void MessageLoop();

	void CleanUp();

#ifdef _DEBUG
	VkResult CheckValidationLayerSupport();
#endif // DEBUG


private:
	bool m_isRunning = false;

	GLFWwindow*				m_window;
	VkInstance				m_instance;
	VkSurfaceKHR			m_surface;
	VkPhysicalDevice		m_physicalDevice	= VK_NULL_HANDLE;
	VkDevice				m_device			= VK_NULL_HANDLE;
	VkQueue					m_graphicsQueue;
	VkQueue					m_presentQueue;
	VkSwapchainKHR			m_swapchain;
	std::vector<VkImage>	m_swapchainImages;
	VkFormat				m_swapchainFormat;
	VkExtent2D				m_swapchainExtent;

#ifdef _DEBUG
	VkDebugUtilsMessengerEXT m_debugMessenger;
#endif // _DEBUG

private:
#ifdef _DEBUG
	static const std::vector<const char*> k_validationLayers;
#endif // _DEBUG

};