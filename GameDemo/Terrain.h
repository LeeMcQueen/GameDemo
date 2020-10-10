#pragma once

#include "RawModel.h"
#include "ModelTexture.h"
#include "Loader.h"

class Terrain
{
public:
	Terrain(int gridX, int gridZ, Loader loader, ModelTexture modeltexture);
	~Terrain();

	const RawModel &get_modol() const;

private:
	const float SIZE = 800;
	const float VERTEX_COUNT = 128;
	const float MAX_HEIGHT = 80.0f;
	const float MIN_HEIGHT = -40.0f;

	float x_;
	float z_;

	RawModel rawmodel_;
	//model use texture,shineDamper,reflectivity
	ModelTexture modelTexture_;
	//buile terrain VAO,vertexnumber
	RawModel generateTerrain(const std::string &fileName);
};

