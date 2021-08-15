#pragma once
#include <iostream>

#include "RawModel.h"
#include "Loader.h"
#include "TerrainTexturePack.h"
#include "TerrainTexture.h"
#include "lodepng.h"

class Terrain
{
public:
	Terrain(int gridX, int gridZ, Loader loader, TerrainTexturePack terrainTexturePack, TerrainTexture blendMap);
	~Terrain();

	float GetX();
	float GetZ();
	RawModel GetModel();
	TerrainTexturePack getTerrainTexturePack();
	TerrainTexture getTerrainTexture();

private:
	const float SIZE = 800;
	const float VERTEX_COUNT = 128;
	const float MAX_HEIGHT = 40.0f;
	const float MIN_HEIGHT = -40.0f;
	const float MAX_PIXEL_COLOUR = 256 * 256 * 256;

	float x_;
	float z_;

	RawModel rawModel_;
	TerrainTexturePack terrainTexturePack_;
	TerrainTexture blendMap_;
	//返回RawModel结构的数组 VAO的ID 顶点的排序
	RawModel generateTerrain(Loader& loader, std::string heightMap);
	void getHeight(int x, int z, char *image);
};

