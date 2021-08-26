#pragma once

#include "WaterShader.h"
#include "Loader.h"
#include "WaterTile.h"
#include "Camera.h"
#include "Light.h"
#include "RawModel.h"
#include "Loader.h"
#include "Maths.h"

class WaterRenderer {

public:

	WaterRenderer(const WaterShader &waterShader, const glm::mat4 &projectionMatrix);

	void Render(std::vector<WaterTile> &waterTile, const Camera &camera, const Light &light);
	void prepareTerrain(WaterTile &waterTile);
	void prepareInstance(WaterTile &waterTile);

private:

	WaterShader waterShader_;
	std::vector<float> vectices_;
	RawModel quad_;
	//加载水面顶点
	void initVertices();
};