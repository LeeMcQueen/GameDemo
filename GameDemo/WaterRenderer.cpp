#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(WaterShader &waterShader, glm::mat4 &projectionMatrix, WaterFrameBuffers &fbo) :
	waterShader_(waterShader),
	fbo_(fbo){

	//loader类加载
	Loader loader;
	//水面DUDV图片加载
	dudvTexture_ = loader.loadTexture(DUDVMAP);
	normalTexture_ = loader.loadTexture(NORMALMAP);

	waterShader_.start();
	waterShader_.loadconnectTextureUnits();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.stop();
}

void WaterRenderer::render(std::vector<WaterTile> &waterTile, Light &sun) {

	for (WaterTile tile : waterTile) {
		prepareWater(tile, sun);
		prepareInstance(tile);
		glDrawElements(GL_TRIANGLES, tile.getModel().getVertexCount(), GL_UNSIGNED_INT, nullptr);
		unbindTextureModel();
	}
}

void WaterRenderer::prepareWater(WaterTile &waterTile, Light &sun) {

	//水面波纹波动
	moveFactor_ += WAVE_SPEED;
	moveFactor_ = std::fmod(moveFactor_, 1);
	waterShader_.loadMoveFactor(moveFactor_);
	//shader光线加载
	waterShader_.loadLightPosition(sun.getPosition());
	waterShader_.loadLightColor(sun.getColor());

	RawModel &rawModel = waterTile.getModel();

	glBindVertexArray(rawModel.getVaoId());
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_.getReflectionTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo_.getRefractionTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvTexture_);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalTexture_);
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