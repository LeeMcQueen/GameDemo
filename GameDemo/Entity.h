#pragma once

#include <gtx\transform.hpp>

#include "struct.h"
#include "TexturedModel.h"
#include "RawModel.h"

class Entity
{
public:
	//constructor
	//Entity(TexturedModel& model,
	//	vector3 position = vector3(0, 0, 0), 
	//	vector3 rotation = vector3(0, 0, 0),
	//	float scale = 1
	//);
	Entity(TexturedModel& model,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 scale
	);

	//destructor
	~Entity();

	//increase position
	void increasePosition(glm::vec3 position);

	//increase rotation
	void increaseRotation(glm::vec3 rotation);

	//Get model texture
	TexturedModel GetModel() const;

	glm::vec3 getPosition() const;

	glm::vec3 getRotation() const;

	glm::vec3 getScale()const;

	void setPosition(glm::vec3 position);

	void setRotation(glm::vec3 rotation);

	void setScale(glm::vec3 scale);

private:

	//Entity position
	glm::vec3 position_;

	//Entity rotation
	glm::vec3 rotation_;

	//scale value
	glm::vec3 scale_;

	//Entity texture
	TexturedModel model_;

	//texture position in an atlas
	//int atlasPosition_;
	
	//Entity raw model
	//RawModel* rawModel_;
};

