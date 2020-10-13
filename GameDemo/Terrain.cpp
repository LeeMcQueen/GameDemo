#include "Terrain.h"

Terrain::Terrain(int gridX, int gridZ, Loader loader, ModelTexture modeltexture)
	:modelTexture_(modeltexture)
	,rawModel_(generateTerrain(loader))
{}

RawModel Terrain::generateTerrain(Loader& loader)
{
	int count = VERTEX_COUNT * VERTEX_COUNT;

	std::vector<glm::vec3> vertices(count), normals(count);
	std::vector<glm::vec2> textureCoords(count);
	std::vector<int> indices(6 * (VERTEX_COUNT - 1) * VERTEX_COUNT);
	int vertexPointer = 0;

	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			vertices[vertexPointer] = glm::vec3(
				(float)j / ((float)VERTEX_COUNT - 1) * SIZE,
				0,
				(float)i / ((float)VERTEX_COUNT - 1) * SIZE);

			normals[vertexPointer] = glm::vec3(0, 1, 0);

			textureCoords[vertexPointer] = glm::vec2(
				(float)j / ((float)VERTEX_COUNT - 1),
				(float)i / ((float)VERTEX_COUNT - 1));

			vertexPointer++;
		}
	}

	int pointer = 0;
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
	{
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
		{
			int topLeft = (gz * VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}
	return loader.loadToVao(vertices, textureCoords, normals, indices);
}

Terrain::~Terrain()
{
}

float Terrain::GetX()
{
	return x_;
}

float Terrain::GetZ()
{
	return z_;
}

RawModel Terrain::GetModel()
{
	return rawModel_;
}

ModelTexture Terrain::GetTexture()
{
	return modelTexture_;
}
