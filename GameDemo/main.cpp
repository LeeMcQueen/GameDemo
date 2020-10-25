
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>

#include "DisplayManager.h"
#include "RawModel.h"
#include "Loader.h"
#include "ModelTexture.h"
#include "TexturedModel.h"
#include "camera.h"
#include "OBJLoader.h"
#include "Light.h"
#include "MasterRenderer.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "GameDemo", NULL, NULL);

	//open window
	DisplayManager* myDisplayManager = new DisplayManager();
	myDisplayManager->CreatManager(window);

	//start temporary data
	Loader loader;

	//camera
	Camera camera;

	MasterRenderer* myMasterRenderer = new MasterRenderer();

	//load ObjModle form objloader function
	OBJLoader* myOBJLoader = new OBJLoader();

	//load OBJModel 3 function
	/*RawModel model = myOBJLoader->tinyOBJLoader("stall");*/
	/*RawModel model = myOBJLoader->loadModel("stall");*/
	RawModel model = myOBJLoader->LoadObjModel("person");

	//load texture use NAME
	ModelTexture texture(loader.loadTexture("playerTexture"));
	texture.setShineDamer(100.0f);
	texture.setReflectivity(1.0f);

	TexturedModel texturedModel(model, texture);

	//model load
	Entity entity(texturedModel, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	//light load
	Light light(glm::vec3(400, 400, 200), glm::vec3(1, 1, 1));
	Terrain terrain(0, 0, loader, ModelTexture(loader.loadTexture("grassy2")));
	Terrain terrain2(100, 100, loader, ModelTexture(loader.loadTexture("grassy3")));

	while (!glfwWindowShouldClose(window))
	{
		//close window
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//transformation
		entity.increasePosition(glm::vec3(0.0f, 0.0f, 0.00f));
		//rotation
		entity.increaseRotation(glm::vec3(0.0f, 0.0001f, 0.0f));

		camera.move();

		myMasterRenderer->processTerrain(terrain2);
		myMasterRenderer->processTerrain(terrain);
		myMasterRenderer->processEntity(entity);

		myMasterRenderer->render(light, camera);

		myMasterRenderer->cleanUp();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	glViewport(0, 0, 1280, 720);
}