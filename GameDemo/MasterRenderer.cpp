#ifndef RENDERENGINE_MASTER_RENDERER_H
#define RENDERENGINE_MASTER_RENDERER_H

#include "MasterRenderer.h"
#include "StaticShader.h"
#include "Renderer.h"

std::map<TexturedModel, std::vector<Entity>> MasterRenderer::entities;

MasterRenderer::MasterRenderer():renderer(shader){}

void MasterRenderer::render(Light & light, Camera & camera)
{
	renderer.Prepare();
	shader.start();

	shader.loadLight(light);
	shader.loadViewMatrix(camera.getViewMatrix());

	renderer.render(entities);

	shader.stop();
	entities.clear();
}

void MasterRenderer::processEntity(const Entity & entity)
{
	entities[entity.GetModel()].push_back(entity);
}

void MasterRenderer::cleanUp()
{
	/*StaticShader* myshader = new StaticShader();*/
	shader.stop();
}

#endif RENDERENGINE_MASTER_RENDERER_H
