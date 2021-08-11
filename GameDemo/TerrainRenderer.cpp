#include "TerrainRenderer.h"

TerrainRenderer::TerrainRenderer(TerrainShader &terrainShader, glm::mat4 &projectionMatrix)
	:terrainShader_(terrainShader)
{
	terrainShader_.start();
	terrainShader_.loadProjectionMatrix(projectionMatrix);
	terrainShader_.loadconnectTextureUnits();
	terrainShader_.stop();
}

void TerrainRenderer::render(std::vector<Terrain> &terrains)
{
	for (Terrain& terrain : terrains)
	{
		prepareTerrain(terrain);
		prepareInstance(terrain);
		glDrawElements(GL_TRIANGLES, terrain.GetModel().getVertexCount(), GL_UNSIGNED_INT, nullptr);
		unbindTextureModel();
	}
}

void TerrainRenderer::prepareTerrain(Terrain &terrain)
{
	//Get rawModel from textredModel
	RawModel& rawModel = terrain.GetModel();
	glBindVertexArray(rawModel.getVaoId());

	//activate the attribute list
	glEnableVertexAttribArray(0);	//vecter
	glEnableVertexAttribArray(1);	//texture
	glEnableVertexAttribArray(2);	//normal

	bindTexture(terrain);

	//load shine 
	//getShineDamer getReflectivity
	terrainShader_.loadShineVariables(1.0f, 0.0f);

}

void TerrainRenderer::bindTexture(Terrain &terrain)
{
	auto terrainTexturePack = terrain.getTerrainTexturePack();

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
	glBindTexture(GL_TEXTURE_2D, terrain.getTerrainTexture().textureID_);
}

void TerrainRenderer::prepareInstance(Terrain &terrain)
{
	//calculate transformation matrix
	glm::mat4 transformationMatrix = Maths::createTransformationMatrix(
		glm::vec3(terrain.GetX(), 1.0f, terrain.GetZ()),
		glm::vec3(0, 0, 0),
		glm::vec3(1, 1, 1));

	//shader transformMatrix
	terrainShader_.loadTransformationMatrix(transformationMatrix);
}

void TerrainRenderer::unbindTextureModel()
{
	//unbind textureModel
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}