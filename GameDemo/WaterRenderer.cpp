#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(const WaterShader &waterShader, const glm::mat4 &projectionMatrix):
	waterShader_(waterShader){

	initVertices();
	Loader loader;
	quad_ = loader.loadToVAO(vectices_, 2);

	waterShader_.start();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.stop();
}

void WaterRenderer::Render(std::vector<WaterTile> &waterTile, const Camera & camera, const Light &light){

	for(WaterTile tile : waterTile) {
		prepareTerrain(tile);
		prepareInstance(tile);
		glDrawArrays(GL_TRIANGLES, 0, quad_.getVertexCount());
	}

}

void WaterRenderer::prepareTerrain(WaterTile & waterTile){

	glBindVertexArray(quad_.getVaoId());
	glEnableVertexAttribArray(0);
}

void WaterRenderer::prepareInstance(WaterTile & waterTile){

	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(
		glm::vec3(waterTile.getX(), waterTile.getHeight(), waterTile.getZ()),
		glm::vec3(0, 0, 0),
		glm::vec3(waterTile.getTileSize(), waterTile.getTileSize(), waterTile.getTileSize()));
}

void WaterRenderer::initVertices(){

	vectices_ = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
}
