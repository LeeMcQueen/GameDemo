#include "WaterTile.h"

WaterTile::WaterTile(float centerX, float centerZ, float height) :
	x_(centerX),
	z_(centerZ),
	height_(height)
	{}
//rawModel_(generateWater(loader))

WaterTile::~WaterTile(){

}

//RawModel WaterTile::generateWater(Loader &loader){
//
//	std::vector<glm::vec2> textureCoords;
//
//	return loader.loadToVAO(textureCoords, 2);
//}
