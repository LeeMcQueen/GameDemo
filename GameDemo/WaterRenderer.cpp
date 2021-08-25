#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(const WaterShader &waterShader, const glm::mat4 &projectionMatrix):
	waterShader_(waterShader){

	waterShader_.start();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.stop();
}

void WaterRenderer::Render(const std::vector<WaterTile> &waterTile, const Camera & camera, const Light &light){


}

void WaterRenderer::initVertices(){

	vectices_ = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
}
