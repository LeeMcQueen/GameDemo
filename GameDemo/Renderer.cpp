#include "Renderer.h"
#include "RawModel.h"
#include "TexturedModel.h"
#include "Maths.h"
#include "DisplayManager.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Renderer::Renderer(StaticShader* shader)
	:projectionMatrix_(getProjectionMatrix())
{
	shader->start();
	shader->loadProjectionMatrix(projectionMatrix_);
	shader->stop();
}

void Renderer::Prepare()
{
	//witch triangle is first to build
	glEnable(GL_DEPTH_TEST);

	//clear COLOR_BUFFER & DEPTH_BUFFER
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 0, 0, 1);
}

void Renderer::Render(Entity entity, StaticShader* shader) {

	//Get textureModel form entity
	TexturedModel& model = entity.GetModel();
	//Get rawModel from textredModel
	RawModel& rawModel = model.GetRawModel();
	//Get texture form ModelTexture
	ModelTexture texture = model.GetTextureModel();

	glBindVertexArray(rawModel.getVaoId());

	//activate the attribute list
	glEnableVertexAttribArray(0);//vecter
	glEnableVertexAttribArray(1);//texture
	glEnableVertexAttribArray(2);//normal

	//calculate transformation matrix
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(entity.getPosition(),
		entity.getRotation(),
		entity.getScale());

	shader->loadTransformationMatrix(transformationMatrix);
	shader->loadShineVariables(texture.getShineDamer(), texture.getReflectivity());

 	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.GetTextureModel().getID());

	//Draw function (MODEL triangles, Draw how many point, type, indices)
	/*glDrawArrays(GL_TRIANGLES, 0, model.getVertexCount());*/
	glDrawElements(GL_TRIANGLES, rawModel.getVertexCount(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

//get&create projectionMatrix
glm::mat4 Renderer::getProjectionMatrix()
{
	return glm::perspective(glm::radians(FOV),
		(float)DisplayManager::WIDTH / (float)DisplayManager::HEIGHT,
		NEAR_PLANE, FAR_PLANE);
}

