#pragma once
#include <iostream>

#include "RawModel.h"
#include "Loader.h"
#include "TerrainTexturePack.h"
#include "TerrainTexture.h"
#include "std_image.h"

class Terrain
{
public:

	Terrain(int gridX, int gridZ, Loader loader, TerrainTexturePack terrainTexturePack, TerrainTexture blendMap);
	~Terrain();

	float getX();
	float getZ();
	RawModel getModel();
	TerrainTexturePack getTerrainTexturePack();
	TerrainTexture getTerrainTexture();

private:
	const float SIZE = 256;
	float VERTEX_COUNT;
	const float MAX_HEIGHT = 40.0f;
	const float MIN_HEIGHT = -40.0f;
	const float MAX_PIXEL_COLOUR = 256 * 256 * 256;

	//图片的宽，高，通道
	int _width, _height, _colorChannels;
	unsigned char *_image;

	float x_;
	float z_;

	RawModel rawModel_;
	TerrainTexturePack terrainTexturePack_;
	TerrainTexture blendMap_;
	//返回RawModel结构的数组 VAO的ID 顶点的排序
	RawModel generateTerrain(Loader& loader, std::string heightMap);
	float getHeight(int x, int z, unsigned char *image);
	glm::vec3 calculateNormal(int x, int z, unsigned char *image);
	std::int32_t getRGBSum(int x, int y);
};

