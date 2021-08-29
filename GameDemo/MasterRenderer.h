#pragma once

#include <map>
#include <vector>

#include "StaticShader.h"
#include "EntityRenderer.h"
#include "TerrainRenderer.h"
#include "WaterRenderer.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"
#include "WaterTIle.h"

class MasterRenderer{

public:

	MasterRenderer();

	void render(Light &light, Camera &camera);
	void processEntity(const Entity &entity);
	void processTerrain(const Terrain &terrain);
	void processWater(const WaterTile &waterTile);
	void prepare();
	void cleanUp();

	glm::mat4 getProjectionMatrix();

private:

	//设置头晕矩阵基本变量
	const float FOV = 70.0f;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 1000.0f;

	//投影矩阵
	glm::mat4 projectionMatrix_;

	//shader
	StaticShader staticshader_;
	TerrainShader terrainShader_;
	WaterShader waterShader_;

	//Renderer
	EntityRenderer entityRenderer_;
	TerrainRenderer terrainRenderer_;
	WaterRenderer waterRenderer_;

	static std::vector<Terrain> terrains_;
	static std::map<TexturedModel, std::vector<Entity>> entities_;
	static std::vector<WaterTile> waterTiles_;
};

