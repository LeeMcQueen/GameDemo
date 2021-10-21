#pragma once

#include "WaterShader.h"
#include "Loader.h"
#include "WaterTile.h"
#include "RawModel.h"
#include "WaterFrameBuffers.h"
#include "Maths.h"

class WaterRenderer {

public:

	WaterRenderer(WaterShader &waterShader, glm::mat4 &projectionMatrix, WaterFrameBuffers &fbo);
	void render(std::vector<WaterTile> &waterTile, Light &sun);

private:
	const char *DUDVMAP = "waterDUDV";
	const char *NORMALMAP = "waterNormalMap";
	float WAVE_SPEED = 0.0003;

	unsigned int dudvTexture_;
	unsigned int normalTexture_;
	float moveFactor_ = 0;

	WaterShader waterShader_{};
	const WaterFrameBuffers &fbo_;

	//加载水面顶点
	void prepareWater(WaterTile &waterTile, Light &sun);
	void prepareInstance(WaterTile &waterTile);
	void unbindTextureModel();
};