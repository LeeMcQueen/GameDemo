#pragma once

#include "Loader.h"
#include "RawModel.h"
#include "TerrainTexturePack.h"
#include "TerrainTexture.h"
#include "std_image.h"

class WaterTile {

public:

	WaterTile(int centerX, int centerZ, Loader loader);
	~WaterTile();

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
	float VERTEX_COUNT = 2;

	const float TILE_SIZE = 125.0f;
	float x_;
	float z_;
	float height_;

	RawModel rawModel_;

	RawModel generateWater(Loader &loader);
};