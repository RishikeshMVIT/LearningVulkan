#include "Application.h"

int main()
{
	Application app = Application();

	app.Initialize();

    try 
    {
        app.Run();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

	app.Shutdown();

	return EXIT_SUCCESS;
}