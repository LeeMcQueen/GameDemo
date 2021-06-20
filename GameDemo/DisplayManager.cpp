#include "DisplayManager.h"

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int DisplayManager::CreatManager(GLFWwindow *window) {

	glfwMakeContextCurrent(window);

	//设置窗口
	glViewport(0, 0, 1280, 720);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		printf("GLEW False");
		glfwTerminate();
		return -1;
	}

	return 0;
}



