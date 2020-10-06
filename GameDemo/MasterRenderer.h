#pragma once

#include "StaticShader.h"
#include "Renderer.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"

#include <map>
#include <vector>

class MasterRenderer
{
public:
	MasterRenderer();

	void render(Light& light, Camera& camera);

	void processEntity(const Entity& entity);

	void cleanUp();

private:

	StaticShader shader;
	Renderer renderer;
	static std::map<TexturedModel, std::vector<Entity>> entities;
};

