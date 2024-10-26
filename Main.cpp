#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

const unsigned int WIDTH = 960;
const unsigned int HEIGHT = 540;

class Application
{
public:
	void Run()
	{
		InitializeWindow();
		InitializeVulkan();
		MainLoop();
		CleanUp();
	}
private:
	void InitializeWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
	}

	void InitializeVulkan()
	{

	}

	void MainLoop()
	{
		while (!glfwWindowShouldClose(_window))
		{
			glfwPollEvents();
		}
	}

	void CleanUp()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}


private:
	GLFWwindow * _window = 0;
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
