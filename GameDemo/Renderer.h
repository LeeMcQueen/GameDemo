#pragma once

#include <gtx\transform.hpp>
#include <map>
#include <vector>

#include "RawModel.h"
#include "TexturedModel.h"
#include "Entity.h"
#include "StaticShader.h"

class Renderer
{
public:

	Renderer(StaticShader& shader);

	void Prepare();

	//void Render(Entity entity, StaticShader* shader);
	void render(const std::map<TexturedModel, std::vector<Entity>>& entities);

private:

	//Set number into projectionMatrix
	const float FOV = 70.0f;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 1000.0f;

	//Get Set Value staticShader
	StaticShader shader_;
	//Get Set Value projectionMatrix
	const glm::mat4 projectionMatrix_;

	glm::mat4 getProjectionMatrix();

	void prepareTextureModel(const TexturedModel& model);
	void prepareInstance(const Entity& entity);
	void unbindTextureModel();
};

