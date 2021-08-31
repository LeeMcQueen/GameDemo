#pragma once

#include "Loader.h"
#include "RawModel.h"

class WaterTile {

public:

	WaterTile(float centerX, float centerZ, float height);
	~WaterTile();

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

	RawModel generateWater(Loader &loader);

	RawModel rawModel_;
	const float TILE_SIZE = 125.0f;
	float x_;
	float z_;
	float height_;
};