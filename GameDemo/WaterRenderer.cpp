#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(WaterShader &waterShader, glm::mat4 &projectionMatrix, WaterFrameBuffers &fbo) :
	waterShader_(waterShader),
	fbo_(fbo){

	Loader loader;

	dudvTexture_ = loader.loadTexture(DUDVMAP);

	waterShader_.start();
	waterShader_.loadconnectTextureUnits();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.stop();
}

void WaterRenderer::render(std::vector<WaterTile> &waterTile) {

	for (WaterTile tile : waterTile) {
		prepareWater(tile);
		prepareInstance(tile);
		//glDrawArrays(GL_TRIANGLES, 0, tile.getModel().getVertexCount());
		glDrawElements(GL_TRIANGLES, tile.getModel().getVertexCount(), GL_UNSIGNED_INT, nullptr);
		unbindTextureModel();
	}

}

void WaterRenderer::prepareWater(WaterTile &waterTile) {

	RawModel &rawModel = waterTile.getModel();

	glBindVertexArray(rawModel.getVaoId());
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_.getReflectionTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_.getRefractionTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvTexture_);
}

void WaterRenderer::prepareInstance(WaterTile & waterTile) {
	//变换矩阵
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(
		glm::vec3(waterTile.getX(), waterTile.getHeight(), waterTile.getZ()),
		glm::vec3(0, 0, 0),
		glm::vec3(waterTile.getTileSize(), waterTile.getTileSize(), waterTile.getTileSize()));

	//shader transformMatrix
	waterShader_.loadTransformationMatrix(transformationMatrix);
}

void WaterRenderer::unbindTextureModel() {
	//unbind textureModel
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}