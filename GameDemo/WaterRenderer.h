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

	void render(std::vector<WaterTile> &waterTile);
private:
	const char *DUDVMAP = "waterDUDV";
	float WAVE_SPEED = 0.03f;

	WaterShader waterShader_;
	const WaterFrameBuffers &fbo_;
	unsigned int dudvTexture_;
	float moveFactor = 0;

	//加载水面顶点
	void prepareWater(WaterTile &waterTile);
	void prepareInstance(WaterTile &waterTile);
	void unbindTextureModel();
};