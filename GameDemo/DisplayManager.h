#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma once
class DisplayManager
{
public:
	constexpr static int WIDTH = 1280;
	constexpr static int HEIGHT = 720;

	int CreatManager(GLFWwindow* window);
};

