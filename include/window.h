#pragma once

#include <string>
#include <glfw3.h>
#include <gl/GL.h>

class Window
{
public:
	Window() {};
	~Window() {};

	bool init(std::string title, int w, int h);
	bool shouldClose();
	
	void update();

private:
	GLFWwindow* window;

};

