#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(WaterShader &waterShader, glm::mat4 &projectionMatrix):
	waterShader_(waterShader) {

	waterShader_.start();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.stop();
}

void WaterRenderer::render(std::vector<WaterTile> &waterTile){

	for(WaterTile tile : waterTile) {
		prepareWater(tile);
		prepareInstance(tile);
		glDrawArrays(GL_TRIANGLES, 0, tile.getModel().getVertexCount());
		unbindTextureModel();
	}

}

void WaterRenderer::prepareWater(WaterTile &waterTile){

	RawModel &rawModel = waterTile.getModel();

	glBindVertexArray(rawModel.getVaoId());
	glEnableVertexAttribArray(0);

}

void WaterRenderer::prepareInstance(WaterTile & waterTile){
	//变换矩阵
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(
		glm::vec3(waterTile.getX(), 1.0f, waterTile.getZ()),
		glm::vec3(90, 0, 0),
		glm::vec3(waterTile.getTileSize(), waterTile.getTileSize(), waterTile.getTileSize()));

	//shader transformMatrix
	waterShader_.loadTransformationMatrix(transformationMatrix);
}

void WaterRenderer::unbindTextureModel(){
	//unbind textureModel
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}
