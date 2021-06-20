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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable alpha blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Antialiasing
	glEnable(GL_MULTISAMPLE);
}

void MasterRenderer::render(Light &light, Camera &camera) {
	Prepare();

	//Entity
	staticshader.start();
	staticshader.loadLight(light);
	staticshader.loadViewMatrix(camera.getViewMatrix());
	entityRenderer_.render(entities);
	staticshader.stop();

	//terrain
	terrainShader.start();
	staticshader.loadLight(light);
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

void MasterRenderer::Prepare()
{
	//witch triangle is first to build
	glEnable(GL_DEPTH_TEST);

	//clear COLOR_BUFFER & DEPTH_BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 0, 0, 1);
}

//get&create projectionMatrix
glm::mat4 MasterRenderer::getProjectionMatrix()
{
	return glm::perspective(glm::radians(FOV),
		(float)DisplayManager::WIDTH / (float)DisplayManager::HEIGHT,
		NEAR_PLANE, FAR_PLANE);
}


void MasterRenderer::cleanUp()
{
	/*StaticShader* myshader = new StaticShader();*/
	staticshader.stop();
	terrainShader.stop();
}

#endif RENDERENGINE_MASTER_RENDERER_H
