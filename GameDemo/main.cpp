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

	Loader* myLoader = new Loader();	

	MasterRenderer* myMasterRenderer = new MasterRenderer();

	//camera
	//Camera* myCamera = new Camera();
	Camera camera;

	//load ObjModle form objloader function
	OBJLoader* myOBJLoader = new OBJLoader();


	/*RawModel model = myOBJLoader->tinyOBJLoader("stall");*/
	/*RawModel model = myOBJLoader->loadModel("stall");*/
	RawModel model = myOBJLoader->LoadObjModel("bunny");

	//load texture use NAME
	ModelTexture texture(myLoader->loadTexture("white"));
	TexturedModel texturedModel(model, texture);

	texture.setShineDamer(100.0f);
	texture.setReflectivity(1.0f);

	Entity entity(texturedModel, glm::vec3(0, -5.0f, -5.0f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	Light light(glm::vec3(400, 400, 200), glm::vec3(1, 1, 1));

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		entity.increasePosition(glm::vec3(0.0f, 0.0f, 0.00f));
		entity.increaseRotation(glm::vec3(0.0f, 0.0001f, 0.0f));

		camera.move();

		myMasterRenderer->processEntity(entity);

		myMasterRenderer->render(light, camera);

		myMasterRenderer->cleanUp();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	glViewport(0, 0, 1280, 720);

}