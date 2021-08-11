#pragma once
#include "TerrainTexture.h"

class TerrainTexturePack {

public:
	TerrainTexturePack(const TerrainTexture backgroundTexture
		, const TerrainTexture rTexture
		, const TerrainTexture gTexture
		, const TerrainTexture bTexture) :
		backGroundTexture_(backgroundTexture),
		rTexture_(rTexture),
		gTexture_(gTexture),
		bTexture_(bTexture) {};

	const TerrainTexture &getBackgroundTexture() const {
		return backGroundTexture_;
	}
	const TerrainTexture &getRTexture() const {
		return rTexture_;
	}
	const TerrainTexture &getGTexture() const {
		return gTexture_;
	}
	const TerrainTexture &getBTexture() const {
		return bTexture_;
	}

private:
	TerrainTexture backGroundTexture_;
	TerrainTexture rTexture_;
	TerrainTexture gTexture_;
	TerrainTexture bTexture_;
};