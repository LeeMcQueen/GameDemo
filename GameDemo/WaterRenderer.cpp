#include "WaterRenderer.h"

namespace {
	const std::vector<GLfloat> POSITIONS
	{ -1, -1, -1,  1, 1, -1, 1, -1, -1,  1, 1,  1 };
}

WaterRenderer::WaterRenderer(Loader &loader, WaterShader &waterShader, glm::mat4 &projectionMatrix):
	waterShader_(waterShader),
	quad_(loader.loadToVAO(POSITIONS, 2)) {

	waterShader_.start();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.stop();
}

void WaterRenderer::render(std::vector<WaterTile> &waterTile, Camera &camera){

	prepareWater(camera);

	for(WaterTile tile : waterTile) {
		glm::mat4 transformationMatrix = Maths::createTransformationMatrix(
			glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()),
			glm::vec3(glfwGetTime(), glfwGetTime(), glfwGetTime()),
			glm::vec3(200.0f, 200.0f, 200.0f));

		waterShader_.loadTransformationMatrix(transformationMatrix);

		glDrawArrays(GL_TRIANGLES, 0, quad_.getVertexCount());
	}

	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	waterShader_.stop();
}

void WaterRenderer::prepareWater(Camera &camera){

	waterShader_.start();
	waterShader_.loadViewMatrix(camera.getViewMatrix());

	glBindVertexArray(quad_.getVaoId());
	glEnableVertexAttribArray(0);

}

void WaterRenderer::initVertices(){

	vectices_ = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
}
