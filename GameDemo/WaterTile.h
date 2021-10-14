#pragma once

#include "Loader.h"
#include "RawModel.h"
#include <iostream>

class WaterTile {

public:

	WaterTile(float centerX, float centerZ, float height,Loader loader);
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

	const float SIZE = 1;
	float VERTEX_COUNT = 2;
	const float TILE_SIZE = 120.0f;
	float x_;
	float z_;
	float height_;

	RawModel rawModel_;

	RawModel generateWater(Loader &loader);
};