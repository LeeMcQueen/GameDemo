#include "EntityRenderer.h"
#include "RawModel.h"
#include "TexturedModel.h"
#include "Maths.h"
#include "DisplayManager.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

EntityRenderer::EntityRenderer(StaticShader &shader, glm::mat4 &projectionMatrix)
	:shader_(shader) {
	shader_.start();
	shader_.loadProjectionMatrix(projectionMatrix);
	shader_.stop();
}

void EntityRenderer::render(const std::map<TexturedModel, std::vector<Entity>> &entities) {
	for (const auto& model : entities)
	{
		prepareTextureModel(model.first);

		auto batch = model.second;
		for (const auto &entity : batch)
		{
			prepareInstance(entity);
			//描画功能 (顶点描画模式, 共有多少个顶点, 类型, indices)
			glDrawElements(GL_TRIANGLES, model.first.GetRawModel().getVertexCount(), GL_UNSIGNED_INT, nullptr);
		}

		unbindTextureModel();
	}
}

void EntityRenderer::prepareTextureModel(const TexturedModel &model) {
	//Get rawModel from textredModel
	RawModel& rawModel = model.GetRawModel();
	glBindVertexArray(rawModel.getVaoId());

	//启动顶点属性列表(共16个)
	glEnableVertexAttribArray(0);	//顶点
	glEnableVertexAttribArray(1);	//纹理
	glEnableVertexAttribArray(2);	//法线

	//Get texture form ModelTexture
	ModelTexture& texture = model.GetTextureModel();
	//加载反光
	shader_.loadShineVariables(texture.getShineDamer(), texture.getReflectivity());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.GetTextureModel().getID());
}

void EntityRenderer::prepareInstance(const Entity & entity) {
	//计算变换举证
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(entity.getPosition(),
		entity.getRotation(),
		entity.getScale());

	//shader的变换矩阵
	shader_.loadTransformationMatrix(transformationMatrix);
}

void EntityRenderer::unbindTextureModel() {
	//unbind textureModel
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

