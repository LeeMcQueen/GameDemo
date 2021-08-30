#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(Loader &loader, WaterShader &waterShader, glm::mat4 &projectionMatrix):
	waterShader_(waterShader){

	waterShader_.start();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.stop();

	initVertices(loader);
	quad_ = loader.FUCKvao(vectices_, 2);
}

void WaterRenderer::render(std::vector<WaterTile> &waterTile, Camera &camera){

	prepareWater(camera);

	for(WaterTile tile : waterTile) {
		glm::mat4 transformationMatrix = Maths::createTransformationMatrix(
			glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()),
			glm::vec3(0, 0, 0),
			glm::vec3(tile.getTileSize(), tile.getTileSize(), tile.getTileSize()));

		waterShader_.loadTransformationMatrix(transformationMatrix);

		glDrawArrays(GL_TRIANGLES, 0, quad_.getVertexCount());
	}

}

void WaterRenderer::prepareWater(Camera &camera){

	waterShader_.start();
	waterShader_.loadViewMatrix(camera.getViewMatrix());

	glBindVertexArray(quad_.getVaoId());
	glEnableVertexAttribArray(0);
	waterShader_.stop();
}

void WaterRenderer::initVertices(Loader &loader){

	vectices_ = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
}
