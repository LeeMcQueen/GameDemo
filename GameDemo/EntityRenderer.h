#pragma once

#include <gtx\transform.hpp>
#include <map>
#include <vector>

#include "RawModel.h"
#include "TexturedModel.h"
#include "Entity.h"
#include "StaticShader.h"

class EntityRenderer
{
public:

	EntityRenderer(StaticShader& shader, glm::mat4 &projectionMatrix);

	//void Render(Entity entity, StaticShader* shader);
	void render(const std::map<TexturedModel, std::vector<Entity>> &entities);

private:

	//Get Set Value staticShader
	StaticShader shader_;

	void prepareTextureModel(const TexturedModel &model);
	void prepareInstance(const Entity &entity);
	void unbindTextureModel();
};

