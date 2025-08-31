#include <window.h>
#include <iostream>

bool Window::init(std::string title, int w, int h)
{
	// Init GLFW
	if (!glfwInit())
	{
		std::cout << "GLFW Couldn't Initialize" << std::endl;
		return false;
	}

	window = glfwCreateWindow(w, h, title.c_str(), NULL ,NULL);

	if (!window)
	{
		std::cout << "Window Wasn't Created" << std::endl;
		glfwTerminate();
		return false;
	}
	
	glfwMakeContextCurrent(window);
	std::cout << "Window Created Succesfully" << std::endl;
	return true;
}

bool Window::shouldClose()
{
	if (glfwWindowShouldClose(window))
		return true;

	return false;
}

void Window::update()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}