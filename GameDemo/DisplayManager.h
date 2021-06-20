#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <string>

#pragma once
class DisplayManager
{
public:
	constexpr static int WIDTH = 1280;
	constexpr static int HEIGHT = 720;

	//创建窗口函数
	static int CreatManager(GLFWwindow *window);

private:

};

