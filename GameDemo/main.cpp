#include <iostream>

#include "DisplayManager.h"
#include "RawModel.h"
#include "Loader.h"
#include "Renderer.h"
#include "ShaderProgram.h"
#include "StaticShader.h"
#include "ModelTexture.h"
#include "TexturedModel.h"
#include "camera.h"
#include "OBJLoader.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
 
//std::vector<float> vertices = {
//	-0.5f,0.5f,0,
//	-0.5f,-0.5f,0,
//	0.5f,-0.5f,0,
//	0.5f,0.5f,0,
//
//	-0.5f,0.5f,1,
//	-0.5f,-0.5f,1,
//	0.5f,-0.5f,1,
//	0.5f,0.5f,1,
//
//	0.5f,0.5f,0,
//	0.5f,-0.5f,0,
//	0.5f,-0.5f,1,
//	0.5f,0.5f,1,
//
//	-0.5f,0.5f,0,
//	-0.5f,-0.5f,0,
//	-0.5f,-0.5f,1,
//	-0.5f,0.5f,1,
//
//	-0.5f,0.5f,1,
//	-0.5f,0.5f,0,
//	0.5f,0.5f,0,
//	0.5f,0.5f,1,
//
//	-0.5f,-0.5f,1,
//	-0.5f,-0.5f,0,
//	0.5f,-0.5f,0,
//	0.5f,-0.5f,1
//};
//
//std::vector<int> indices = {
//	0,1,3,
//	3,1,2,
//	4,5,7,
//	7,5,6,
//	8,9,11,
//	11,9,10,
//	12,13,15,
//	15,13,14,
//	16,17,19,
//	19,17,18,
//	20,21,23,
//	23,21,22
//};
//
//std::vector<float> textureCoords = {
//	0,0,
//	0,1,
//	1,1,
//	1,0,
//	0,0,
//	0,1,
//	1,1,
//	1,0,
//	0,0,
//	0,1,
//	1,1,
//	1,0,
//	0,0,
//	0,1,
//	1,1,
//	1,0,
//	0,0,
//	0,1,
//	1,1,
//	1,0,
//	0,0,
//	0,1,
//	1,1,
//	1,0
//};

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "GameDemo", NULL, NULL);

	DisplayManager* myDisplayManager = new DisplayManager();
	myDisplayManager->CreatManager(window);

	Loader* myLoader = new Loader();	
	StaticShader* myShader = new StaticShader();
	Renderer* myRenderer = new Renderer(myShader);
	Camera* myCamera = new Camera();
	OBJLoader* myOBJLoader = new OBJLoader();

	/*RawModel model = myLoader->loadToVAO(vertices, textureCoords, indices);*/
	RawModel model = myOBJLoader->loadOBJ("fern");
	//load texture use NAME
	ModelTexture texture(myLoader->loadTexture("fern"));
	TexturedModel texturedModel(model, texture);


	Entity entity(texturedModel, glm::vec3(0, 0, 0.0f), glm::vec3(0, 0.5f, 0.5f), glm::vec3(1, 1, 1));

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		entity.increasePosition(glm::vec3(0.0f, 0.0f, 0.00f));
		entity.increaseRotation(glm::vec3(0.0f, 0.001f, 0.0f));

		myCamera->move();

		myRenderer->Prepare();

		//Ê¹ÓÃµÄShaderPrograme
		myShader->start();

		myShader->loadViewMatrix(myCamera->getViewMatrix());

		myRenderer->Render(entity, myShader);

		myShader->stop();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	glViewport(0, 0, 1280, 720);

}