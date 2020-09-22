#include <iostream>

#include "DisplayManager.h"
#include "RawModel.h"
#include "Loader.h"
#include "Renderer.h"
#include "ShaderProgram.h"
#include "StaticShader.h"
#include "ModelTexture.h"
#include "TexturedModel.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
 
std::vector<float> vertices = {
	// 第一个三角形
	-0.5f, 0.5f, 0.0f,  // 右上角
	-0.5f, -0.5f, 0.0f, // 右下角
	0.5f, -0.5f, 0.0f,   // 左上角
	0.5f,0.5f,0.0f
};

std::vector<int> indices = {
	0,1,3,
	3,1,2
};

std::vector<float> textureCoords = {
	0, 0,
	0, 1,
	1, 1,
	1, 0
};

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "GameDemo", NULL, NULL);

	DisplayManager* myDisplayManager = new DisplayManager();
	myDisplayManager->CreatManager(window);

	Loader* myLoader = new Loader();
	Renderer* myRenderer = new Renderer();
	StaticShader* myShader = new StaticShader();

	RawModel model = myLoader->loadToVAO(vertices, textureCoords, indices);
	ModelTexture texture(myLoader->loadTexture("g"));
	TexturedModel texturedModel(model, texture);
	Entity entity(texturedModel, glm::vec3(0,0,0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//entity.increasePosition(0.00000f, 0.0f, 0.0f);
		//entity.increaseRotation(0.0f, 0.001f, 0.0f);

		myRenderer->Prepare();

		//使用的ShaderPrograme
		myShader->start();

		myRenderer->Render(entity, myShader);

		myShader->stop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	glViewport(0, 0, 1280, 720);

}