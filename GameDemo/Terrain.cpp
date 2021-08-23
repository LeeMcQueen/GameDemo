#include "Terrain.h"

Terrain::Terrain(int gridX, int gridZ, Loader loader, TerrainTexturePack terrainTexturePack, TerrainTexture blendMap)
	:terrainTexturePack_(terrainTexturePack),
	blendMap_(blendMap),
	rawModel_(generateTerrain(loader, "heightmap")) {}

RawModel Terrain::generateTerrain(Loader& loader, std::string heightMap)
{
	//_image = stbi_load(("res/" + heightMap + ".png").c_str(), &_width, &_height, &_colorChannels, 0);
	_image = stbi_load("res/heightmap.png", &_width, &_height, &_colorChannels, 0);

	VERTEX_COUNT = _height;

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
				getHeight(j, i, _image),
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

Terrain::~Terrain() {

}

float Terrain::getHeight(int x, int z, unsigned char *data) {
	if (x < 0 || x > _height || z < 0 || z > _height) {
		return 0;
	}

	float height = getRGBSum(x, z);
	height /= (MAX_PIXEL_COLOUR / 2);
	height -= 1.0;
	height *= MAX_HEIGHT;

	return height;
}

std::int32_t Terrain::getRGBSum(int x, int y)
{
	int addr = (y * _width + x) * _colorChannels;
	std::int32_t r = _image[addr];
	std::int32_t g = _image[addr + 1];
	std::int32_t b = _image[addr + 2];
	return (r << 16) + (g << 8) + b;
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

TerrainTexturePack Terrain::getTerrainTexturePack()
{
	return terrainTexturePack_;
}

TerrainTexture Terrain::getTerrainTexture()
{
	return blendMap_;
}
