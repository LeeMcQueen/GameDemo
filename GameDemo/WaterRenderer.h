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

	WaterRenderer(WaterShader &waterShader, glm::mat4 &projectionMatrix);

	void render(std::vector<WaterTile> &waterTile);

private:

	WaterShader waterShader_;

	//加载水面顶点
	void prepareWater(WaterTile &waterTile);
	void prepareInstance(WaterTile &waterTile);
	void unbindTextureModel();
};