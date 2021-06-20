#pragma once

#include "StaticShader.h"
#include "EntityRenderer.h"
#include "TerrainRenderer.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"

#include <map>
#include <vector>

class MasterRenderer
{
public:
	MasterRenderer();

	void render(Light &light, Camera &camera);

	void processEntity(const Entity &entity);
	void processTerrain(const Terrain &terrain);

	glm::mat4 getProjectionMatrix();

	void Prepare();

	void cleanUp();

private:

	//Set number into projectionMatrix
	const float FOV = 70.0f;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 1000.0f;

	//Get Set Value projectionMatrix
	glm::mat4 projectionMatrix_;

	StaticShader staticshader;
	TerrainShader terrainShader;

	//entityRenderer
	EntityRenderer entityRenderer_;
	TerrainRenderer terrainRenderer_;

	static std::vector<Terrain> terrains;
	static std::map<TexturedModel, std::vector<Entity>> entities;
};

