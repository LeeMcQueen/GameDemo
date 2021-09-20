#include "WaterTile.h"

WaterTile::WaterTile(float centerX, float centerZ, float height, Loader loader) :
	x_(centerX),
	z_(centerZ),
	height_(height),
	rawModel_(generateWater(loader)) {}

WaterTile::~WaterTile() {

}

RawModel WaterTile::generateWater(Loader &loader) {

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
				height_,
				(float)i / ((float)VERTEX_COUNT - 1) * SIZE);
			vertexPointer++;

			std::cout << " X : " << (float)j / ((float)VERTEX_COUNT - 1) * SIZE << std::endl;
			std::cout << " Z : " << (float)i / ((float)VERTEX_COUNT - 1) * SIZE << std::endl;
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