#include "Renderer.h"
#include "RawModel.h"
#include "TexturedModel.h"
#include "Maths.h"
#include "DisplayManager.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Renderer::Renderer(StaticShader& shader)
	:projectionMatrix_(getProjectionMatrix())
	,shader_(shader)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	shader.start();
	shader.loadProjectionMatrix(projectionMatrix_);
	shader.stop();
}

void Renderer::Prepare()
{
	//witch triangle is first to build
	glEnable(GL_DEPTH_TEST);

	//clear COLOR_BUFFER & DEPTH_BUFFER
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 0, 0, 1);
}

void Renderer::render(const std::map<TexturedModel, std::vector<Entity>>& entities)
{
	for (const auto& model : entities)
	{
		prepareTextureModel(model.first);

		auto batch = model.second;
		for (const auto &entity : batch)
		{
			prepareInstance(entity);
			//Draw function (MODEL triangles, Draw how many point, type, indices)
			glDrawElements(GL_TRIANGLES, model.first.GetRawModel().getVertexCount(), GL_UNSIGNED_INT, nullptr);
		}

		unbindTextureModel();
	}
}

void Renderer::prepareTextureModel(const TexturedModel & model)
{
	//Get rawModel from textredModel
	RawModel& rawModel = model.GetRawModel();
	glBindVertexArray(rawModel.getVaoId());

	//activate the attribute list
	glEnableVertexAttribArray(0);	//vecter
	glEnableVertexAttribArray(1);	//texture
	glEnableVertexAttribArray(2);	//normal

								 //Get texture form ModelTexture
	ModelTexture& texture = model.GetTextureModel();

	//load shine
	shader_.loadShineVariables(texture.getShineDamer(), texture.getReflectivity());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.GetTextureModel().getID());
}

void Renderer::prepareInstance(const Entity & entity)
{
	//Get textureModel form entity
	TexturedModel& model = entity.GetModel();

	//calculate transformation matrix
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(entity.getPosition(),
		entity.getRotation(),
		entity.getScale());

	//shader transformMatrix
	shader_.loadTransformationMatrix(transformationMatrix);
}

void Renderer::unbindTextureModel()
{
	//unbind textureModel
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

//get&create projectionMatrix
glm::mat4 Renderer::getProjectionMatrix()
{
	return glm::perspective(glm::radians(FOV),
		(float)DisplayManager::WIDTH / (float)DisplayManager::HEIGHT,
		NEAR_PLANE, FAR_PLANE);
}

//void Renderer::Render(Entity entity, StaticShader* shader) {
//	//Draw function (MODEL triangles, Draw how many point, type, indices)
//	//glDrawElements(GL_TRIANGLES, rawModel.getVertexCount(), GL_UNSIGNED_INT, 0);
//}

