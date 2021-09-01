#include "WaterTile.h"

WaterTile::WaterTile(float centerX, float centerZ, float height, Loader loader) :
	x_(centerX),
	z_(centerZ),
	height_(height),
	rawModel_(generateWater(loader)) {}

WaterTile::~WaterTile(){

}


RawModel WaterTile::generateWater(Loader &loader){

	int count = 6;
	std::vector<glm::vec2> position(count);

	position[0] = glm::vec2(-1, -1);
	position[1] = glm::vec2(-1, 1);
	position[2] = glm::vec2(1, -1);
	position[3] = glm::vec2(1, -1);
	position[4] = glm::vec2(-1, 1);
	position[5] = glm::vec2(1, 1);

	return loader.loadToVAO(position, 2);
}
