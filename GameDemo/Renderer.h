#pragma once

#include <gtx\transform.hpp>

#include "RawModel.h"
#include "TexturedModel.h"
#include "Entity.h"
#include "StaticShader.h"

class Renderer
{
public:

	Renderer(StaticShader* shader);

	void Prepare();

	void Render(Entity entity, StaticShader* shader);

private:

	//Set number into projectionMatrix
	const float FOV = 70.0f;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 1000.0f;

	const glm::mat4 projectionMatrix_;

	glm::mat4 getProjectionMatrix();
};

