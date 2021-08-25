#pragma once

class WaterTile {

public:

	WaterTile(float centerX, float centerZ, float height) :
		x_(centerX),
		z_(centerZ),
		height_(height) {};

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

	const float TILE_SIZE = 125.0f;
	float x_;
	float z_;
	float height_;
};