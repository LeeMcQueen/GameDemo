#pragma once

#include "WaterShader.h"
#include "Loader.h"
#include "WaterTile.h"
#include "Camera.h"
#include "Light.h"
#include "RawModel.h"
#include "Loader.h"
#include "Maths.h"

class WaterRenderer{

public:

	WaterRenderer(Loader &loader ,WaterShader &waterShader, glm::mat4 &projectionMatrix);
	WaterRenderer() = default;

	void render(std::vector<WaterTile> &waterTile, Camera &camera);


private:

	WaterShader waterShader_;
	std::vector<float> vectices_;
	RawModel quad_;
	//加载水面顶点
	void initVertices(Loader &loader);
	void prepareWater(Camera &camera);
};