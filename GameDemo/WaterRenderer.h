#pragma once

#include "WaterShader.h"
#include "Loader.h"
#include "WaterTile.h"
#include "Camera.h"
#include "Light.h"
#include "RawModel.h"

class WaterRenderer {

public:

	WaterRenderer(const WaterShader &waterShader, const glm::mat4 &projectionMatrix);

	void Render(const std::vector<WaterTile> &waterTile, const Camera &camera, const Light &light);

private:

	WaterShader waterShader_;
	std::vector<float> vectices_;
	//加载水面顶点
	void initVertices();
};