#include "WaterRenderer.h"

WaterRenderer::WaterRenderer(WaterShader &waterShader, glm::mat4 &projectionMatrix):
	waterShader_(waterShader) {

	waterShader_.start();
	waterShader_.loadProjectionMatrix(projectionMatrix);
	waterShader_.loadconnectTextureUnits();
	waterShader_.stop();
}

void WaterRenderer::render(std::vector<WaterTile> &waterTile){

	for(WaterTile tile : waterTile) {
		prepareWater(tile);
		prepareInstance(tile);
		//glDrawArrays(GL_TRIANGLES, 0, tile.getModel().getVertexCount());
		glDrawElements(GL_TRIANGLES, tile.getModel().getVertexCount(), GL_UNSIGNED_INT, nullptr);
		unbindTextureModel();
	}

}

void WaterRenderer::prepareWater(WaterTile &waterTile){

	RawModel &rawModel = waterTile.getModel();

	glBindVertexArray(rawModel.getVaoId());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);	//texture
	glEnableVertexAttribArray(2);	//normal

	bindTexture(waterTile);

	//load shine 
	//getShineDamer getReflectivity
	waterShader_.loadShineVariables(1.0f, 0.0f);
}

void WaterRenderer::bindTexture(WaterTile &waterTile)
{
	auto terrainTexturePack = waterTile.getTerrainTexturePack();

	//启动纹理通道
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTexturePack.getBackgroundTexture().textureID_);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainTexturePack.getRTexture().textureID_);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, terrainTexturePack.getGTexture().textureID_);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, terrainTexturePack.getBTexture().textureID_);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, waterTile.getTerrainTexture().textureID_);
}

void WaterRenderer::prepareInstance(WaterTile & waterTile){
	//变换矩阵
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(
		glm::vec3(waterTile.getX(), 0.0f, waterTile.getZ()),
		glm::vec3(0, 0, 0),
		glm::vec3(waterTile.getTileSize(), waterTile.getTileSize(), waterTile.getTileSize()));

	//shader transformMatrix
	waterShader_.loadTransformationMatrix(transformationMatrix);
}

void WaterRenderer::unbindTextureModel(){
	//unbind textureModel
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}
