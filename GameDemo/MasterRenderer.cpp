#ifndef RENDERENGINE_MASTER_RENDERER_H
#define RENDERENGINE_MASTER_RENDERER_H

#include <gtx\transform.hpp>

#include "MasterRenderer.h"
#include "StaticShader.h"
#include "EntityRenderer.h"
#include "DisplayManager.h"

std::map<TexturedModel, std::vector<Entity>> MasterRenderer::entities;
std::vector<Terrain>  MasterRenderer::terrains;

MasterRenderer::MasterRenderer()
	: projectionMatrix_(getProjectionMatrix())
	, entityRenderer_(EntityRenderer(staticshader, projectionMatrix_))
	, terrainRenderer_(TerrainRenderer(terrainShader, projectionMatrix_)) {
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
	staticshader.start();
	staticshader.loadSkyColour(0.5f, 0.1f, 0.2f);
	staticshader.loadLight(light);
	staticshader.loadViewMatrix(camera.getViewMatrix());
	entityRenderer_.render(entities);
	staticshader.stop();

	//terrain
	terrainShader.start();
	terrainShader.loadLight(light);
	terrainShader.loadViewMatrix(camera.getViewMatrix());
	terrainRenderer_.render(terrains);
	terrainShader.stop();

	terrains.clear();
	entities.clear();
}

void MasterRenderer::processEntity(const Entity &entity)
{
	//Get textureModel form entity
	TexturedModel& model = entity.GetModel();

	entities[entity.GetModel()].push_back(entity);
}

void MasterRenderer::processTerrain(const Terrain &terrain)
{
	terrains.push_back(terrain);
}

void MasterRenderer::prepare()
{
	//启动深度缓存(只会渲染没有遮挡的顶点)
	glEnable(GL_DEPTH_TEST);
	//清除 COLOR_BUFFER & DEPTH_BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//添加背景颜色
	glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
}

//返回投影矩阵(getProjectionMatrix)
glm::mat4 MasterRenderer::getProjectionMatrix()
{
	return glm::perspective(glm::radians(FOV),(float)DisplayManager::WIDTH / (float)DisplayManager::HEIGHT,NEAR_PLANE, FAR_PLANE);
}

//清除shader
void MasterRenderer::cleanUp()
{
	staticshader.stop();
	terrainShader.stop();
}

#endif RENDERENGINE_MASTER_RENDERER_H
