#pragma once

#include "struct.h"
#include "ModelTexture.h"
#include "RawModel.h"

class Entity
{
public:
	//constructor
	Entity(ModelTexture& texture,
		vector3 position_ = vector3(0, 0, 0), 
		vector3 rotation_ = vector3(0, 0, 0),
		float scale_ = 1,
		int atlasPosition_ = 0
	);

	//destructor
	~Entity();

private:

	//Entity position
	vector3 position;

	//Entity rotation
	vector3 rotation;

	//scale value
	float scale;

	//texture position in an atlas
	int atlasPosition;
	ModelTexture texture;

	//Entity texture
	/*ModelTexture* texture;*/

	//Entity raw model
	RawModel* model;
};

