#pragma once

#include "Terrain.h"
#include "TerrainShader.h"

class TerrainRenderer{

public:
	
	TerrainRenderer(TerrainShader &terrainShader, glm::mat4 &projectionMatrix);

	void render(std::vector<Terrain> &terrains);

private:

	void prepareTerrain(Terrain &terrain);
	void prepareInstance(Terrain &terrain);
	void bindTexture(Terrain &terrain);
	void unbindTextureModel();

	//Get shader information
	TerrainShader terrainShader_;
};

