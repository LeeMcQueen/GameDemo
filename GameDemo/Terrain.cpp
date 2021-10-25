#include "Terrain.h"

Terrain::Terrain(int gridX, int gridZ, Loader loader, TerrainTexturePack terrainTexturePack, TerrainTexture blendMap)
	:x_(gridX),
	z_(gridZ),
	terrainTexturePack_(terrainTexturePack),
	blendMap_(blendMap),
	rawModel_(generateTerrain(loader, "heightmap")) {}

Terrain::~Terrain() {

}

RawModel Terrain::generateTerrain(Loader& loader, std::string heightMap){

	_image = stbi_load("res/heightmap4.png", &width_, &height_, &colorChannels_, 0);
	VERTEX_COUNT = height_;

	int count = VERTEX_COUNT * VERTEX_COUNT;
	std::vector<glm::vec3> vertices(count), normals(count);
	std::vector<glm::vec2> textureCoords(count);
	std::vector<int> indices(6 * (VERTEX_COUNT - 1) * VERTEX_COUNT);
	int vertexPointer = 0;

	heights_.resize(VERTEX_COUNT);
	for (auto& heightTemp : heights_)
		heightTemp.resize(VERTEX_COUNT);

	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			//高度保存到list
			float height = getHeight(j, i, _image);
			heights_[j][i] = height;

			vertices[vertexPointer] = glm::vec3(
				(float)j / ((float)VERTEX_COUNT - 1) * SIZE,
				height,
				(float)i / ((float)VERTEX_COUNT - 1) * SIZE);

			normals[vertexPointer] = calculateNormal(j, i, _image);

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

float Terrain::getHeight(int x, int z, unsigned char *data) {

	if (x < 0 || x > height_ || z < 0 || z > height_) {
		return 0;
	}

	float height = getRGBSum(x, z);
	height /= (MAX_PIXEL_COLOUR / 2);
	height -= 1.0;
	height *= MAX_HEIGHT;

	return height;
}

std::int32_t Terrain::getRGBSum(int x, int y){

	int addr = (y * width_ + x) * colorChannels_;
	std::int32_t r = _image[addr];
	std::int32_t g = _image[addr + 1];
	std::int32_t b = _image[addr + 2];
	return (r << 16) + (g << 8) + b;
}

float Terrain::getHeightOfTerrain(float worldX, float worldZ){

	//x_，z_是transformationMatirx的偏移量
	float terrainX = worldX - x_;
	float terrainZ = worldX - z_;
	float gridSquareSize = SIZE / static_cast<float>(heights_.size() - 1);

	int gridX = static_cast<int>(std::floor(terrainX / gridSquareSize));
	int gridZ = static_cast<int>(std::floor(terrainZ / gridSquareSize));

	//检测是否是在地形内 超出就返回0为高度
	if (gridX >= heights_.size() - 1 || gridZ >= heights_.size() - 1 || gridX < 0 || gridZ < 0) {
		return 0;
	}

	float xCoord = std::fmod(terrainX, gridSquareSize) / gridSquareSize;
	float zCoord = std::fmod(terrainZ, gridSquareSize) / gridSquareSize;

	float answer;
	Maths maths;

	//上半部三角形
	if (xCoord <= (1.f - zCoord)) {
		answer = Maths::barryCentricInterpolation(glm::vec3{ 0, heights_[gridX][gridZ], 0 },
			glm::vec3{ 1, heights_[gridX + 1][gridZ], 0 },
			glm::vec3{ 0, heights_[gridX][gridZ + 1], 1 },
			glm::vec2{ xCoord, zCoord });
	}
	//下半部三角形
	else {
		answer = Maths::barryCentricInterpolation(glm::vec3{ 0, heights_[gridX + 1][gridZ], 0 },
			glm::vec3{ 1, heights_[gridX + 1][gridZ + 1], 1 },
			glm::vec3{ 0, heights_[gridX][gridZ + 1], 1 },
			glm::vec2{ xCoord, zCoord });
	}

	return answer;
}

glm::vec3 Terrain::calculateNormal(int x, int z, unsigned char * image){

	float heightL = getHeight(x - 1, z, image);
	float heightR = getHeight(x + 1, z, image);
	float heightD = getHeight(x, z - 1, image);
	float heightU = getHeight(x, z + 1, image);
	glm::vec3 normal = {
		heightL - heightR,
		2.f,
		heightD - heightU
	};

	normal = glm::normalize(normal);
	return normal;
}

float Terrain::getX(){

	return x_;
}

float Terrain::getZ(){

	return z_;
}

RawModel Terrain::getModel(){

	return rawModel_;
}

TerrainTexturePack Terrain::getTerrainTexturePack(){

	return terrainTexturePack_;
}

TerrainTexture Terrain::getTerrainTexture(){

	return blendMap_;
}
