#pragma once

#include <gtx\transform.hpp>

#include "MasterRenderer.h"
#include "StaticShader.h"
#include "EntityRenderer.h"
#include "DisplayManager.h"

std::map<TexturedModel, std::vector<Entity>> MasterRenderer::entities_;
std::vector<Terrain>  MasterRenderer::terrains_;
std::vector<WaterTile> MasterRenderer::waterTiles_;

MasterRenderer::MasterRenderer()
	: projectionMatrix_(getProjectionMatrix())
	, entityRenderer_(EntityRenderer(staticshader_, projectionMatrix_))
	, terrainRenderer_(TerrainRenderer(terrainShader_, projectionMatrix_))
	, waterRenderer_(WaterRenderer(waterShader_, projectionMatrix_)) {

	//背面剔除
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// 开启混合通道(Enable alpha blend)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// 开启抗锯齿(Enable Antialiasing)
	glEnable(GL_MULTISAMPLE);
}

void MasterRenderer::render(Light &light, Camera &camera) {

	prepare();

	//Entity
	staticshader_.start();
	staticshader_.loadSkyColour(0.6f, 0.6f, 0.6f);
	staticshader_.loadLight(light);
	staticshader_.loadViewMatrix(camera.getViewMatrix());
	entityRenderer_.render(entities_);
	staticshader_.stop();

	//terrain
	terrainShader_.start();
	terrainShader_.loadLight(light);
	terrainShader_.loadViewMatrix(camera.getViewMatrix());
	terrainRenderer_.render(terrains_);
	terrainShader_.stop();

	//water
	waterShader_.start();
	waterShader_.loadLight(light);
	waterShader_.loadViewMatrix(camera.getViewMatrix());
	waterRenderer_.render(waterTiles_);
	waterShader_.stop();

	terrains_.clear();
	entities_.clear();
	waterTiles_.clear();
}

//主要模型 往主要模型list里注入对象
void MasterRenderer::processEntity(const Entity &entity){
	//Get textureModel form entity
	TexturedModel& model = entity.GetModel();

	entities_[entity.GetModel()].push_back(entity);
}
//地面 往地面list里注入对象
void MasterRenderer::processTerrain(const Terrain &terrain){

	terrains_.push_back(terrain);
}
//水面 往水面list里注入对象
void MasterRenderer::processWater(const WaterTile & waterTile){

	waterTiles_.push_back(waterTile);
}

void MasterRenderer::prepare()
{
	//启动深度缓存(只会渲染没有遮挡的顶点)
	glEnable(GL_DEPTH_TEST);
	//清除 COLOR_BUFFER & DEPTH_BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//添加背景颜色
	glClearColor(0.5f, 0.6f, 0.6f, 1.0f);
}

//返回投影矩阵(getProjectionMatrix)
glm::mat4 MasterRenderer::getProjectionMatrix()
{
	return glm::perspective(glm::radians(FOV), (float)DisplayManager::WIDTH / (float)DisplayManager::HEIGHT, NEAR_PLANE, FAR_PLANE);
}

//清除shader
void MasterRenderer::cleanUp()
{
	staticshader_.stop();
	terrainShader_.stop();
	waterShader_.stop();
}

#endif RENDERENGINE_MASTER_RENDERER_H
