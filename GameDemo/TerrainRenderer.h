#pragma once

#include "Terrain.h"
#include "TerrainShader.h"
#include "ShadowFrameBuffer.h"

class TerrainRenderer{

public:
	
	TerrainRenderer(TerrainShader &terrainShader, glm::mat4 &projectionMatrix, ShadowFrameBuffer &ShadowFBO);
	void render(std::vector<Terrain> &terrains);

private:

	//Get shader information
	TerrainShader terrainShader_;
	const ShadowFrameBuffer shadowFBO_;

	void prepareTerrain(Terrain &terrain);
	void prepareInstance(Terrain &terrain);
	void bindTexture(Terrain &terrain);
	void unbindTextureModel();
};

