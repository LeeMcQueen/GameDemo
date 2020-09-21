#pragma once

#include "struct.h"
#include "TexturedModel.h"
#include "RawModel.h"

class Entity
{
public:
	//constructor
	Entity(TexturedModel& model,
		vector3 position = vector3(0, 0, 0), 
		vector3 rotation = vector3(0, 0, 0),
		float scale = 1
	);

	//destructor
	~Entity();

	//increase position
	void increasePosition(float x_, float y_, float z_);

	//increase rotation
	void increaseRotation(float pitch_, float yaw_, float roll_);

	//Get model texture
	TexturedModel GetTexture() const;

	vector3 getPosition() const;

	vector3 getRotation() const;

	float getScale()const;

	void setPosition(vector3 position);
	void setRotation(vector3 rotation);
	void setScale(float scale);

private:

	//Entity position
	vector3 position_;

	//Entity rotation
	vector3 rotation_;

	//scale value
	float scale_;

	//texture position in an atlas
	//int atlasPosition_;
	
	//Entity texture
	TexturedModel model_;

	//Entity raw model
	//RawModel* rawModel_;
};

