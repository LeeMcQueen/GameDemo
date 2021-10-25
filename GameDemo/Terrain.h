#pragma once
#include <iostream>

#include "RawModel.h"
#include "Loader.h"
#include "TerrainTexturePack.h"
#include "TerrainTexture.h"
#include "std_image.h"
#include "Maths.h"

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

	//得到地形高度
	float getHeightOfTerrain(float worldX, float worldZ);

private:
	const float SIZE = 400;
	float VERTEX_COUNT;
	const float MAX_HEIGHT = 40.0f;
	const float MIN_HEIGHT = -40.0f;
	const float MAX_PIXEL_COLOUR = 256 * 256 * 256;

	//图片的宽，高，通道
	int width_, height_, colorChannels_;
	unsigned char *_image;

	float x_, z_;
	std::vector<std::vector<float>> heights_;

	RawModel rawModel_;
	TerrainTexturePack terrainTexturePack_;
	TerrainTexture blendMap_;

	//返回RawModel结构的数组 VAO的ID 顶点的排序
	RawModel generateTerrain(Loader& loader, std::string heightMap);
	glm::vec3 calculateNormal(int x, int z, unsigned char *image);
	std::int32_t getRGBSum(int x, int y);
	float getHeight(int x, int z, unsigned char *image);
};

