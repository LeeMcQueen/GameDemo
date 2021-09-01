#pragma once

#include "Loader.h"
#include "RawModel.h"
#include "TerrainTexturePack.h"
#include "TerrainTexture.h"
#include "std_image.h"

class WaterTile {

public:

	WaterTile(int centerX, int centerZ, Loader loader, TerrainTexturePack terrainTexturePack, TerrainTexture blendMap);
	~WaterTile();

	TerrainTexturePack getTerrainTexturePack();
	TerrainTexture getTerrainTexture();

	RawModel getModel() {
		return rawModel_;
	};

	float getX() const {
		return x_;
	}

	float getZ() const {
		return z_;
	}

	float getHeight() const {
		return height_;
	}

	float getTileSize() const {
		return TILE_SIZE;
	}
private:

	const float SIZE = 800;
	float VERTEX_COUNT = 128;
	
	const float TILE_SIZE = 125.0f;
	float x_;
	float z_;
	float height_;

	//图片的宽，高，通道
	TerrainTexturePack terrainTexturePack_;
	TerrainTexture blendMap_;
	const float MAX_HEIGHT = 40.0f;
	const float MIN_HEIGHT = -40.0f;
	const float MAX_PIXEL_COLOUR = 256 * 256 * 256;
	int _width, _height, _colorChannels;
	unsigned char *_image;

	RawModel rawModel_;

	RawModel generateWater(Loader &loader, std::string heightMap);
	float getHeight(int x, int z, unsigned char *image);
	glm::vec3 calculateNormal(int x, int z, unsigned char *image);
	std::int32_t getRGBSum(int x, int y);
};