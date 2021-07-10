#include "TerrainRenderer.h"

TerrainRenderer::TerrainRenderer(TerrainShader& terrainShader, glm::mat4& projectionMatrix)
	:terrainShader_(terrainShader)
{
	terrainShader_.start();
	terrainShader_.loadProjectionMatrix(projectionMatrix);
	terrainShader_.stop();
}

void TerrainRenderer::render(std::vector<Terrain>& terrains)
{
	for (Terrain& terrain : terrains)
	{
		prepareTerrain(terrain);
		prepareInstance(terrain);
		glDrawElements(GL_TRIANGLES, terrain.GetModel().getVertexCount(), GL_UNSIGNED_INT, nullptr);
		unbindTextureModel();
	}
}

void TerrainRenderer::prepareTerrain(Terrain& terrain)
{
	//Get rawModel from textredModel
	RawModel& rawModel = terrain.GetModel();
	glBindVertexArray(rawModel.getVaoId());

	//activate the attribute list
	glEnableVertexAttribArray(0);	//vecter
	glEnableVertexAttribArray(1);	//texture
	glEnableVertexAttribArray(2);	//normal

	//Get texture form ModelTexture
	ModelTexture& texture = terrain.GetTexture();

	//load shine
	terrainShader_.loadShineVariables(texture.getShineDamer(), texture.getReflectivity());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrain.GetTexture().getID());
}

void TerrainRenderer::prepareInstance(Terrain& terrain)
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