#include "Entity.h"

#include <iostream>

using namespace std;

Entity::Entity(TexturedModel& model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	:model_(model),
	position_(position),
	rotation_(rotation),
	scale_(scale)
{
	
}

Entity::~Entity()
{

}

void Entity::increasePosition(glm::vec3 position)
{
	position_ += position;

	//cerr << "1.transformation:" << position_.x << endl;
	//cerr << "2.transformation:" << position_.y << endl;
	//cerr << "3.transformation:" << position_.z << endl;
}

void Entity::increaseRotation(glm::vec3 rotation)
{
	rotation_ += rotation;

	//cerr << "4.rotation:" << rotation_.x << endl;
	//cerr << "5.rotation:" << rotation_.y << endl;
	//cerr << "6.rotation:" << rotation_.z << endl;
}

TexturedModel Entity::GetTexture() const
{
	return model_;
}

glm::vec3 Entity::getPosition() const
{
	return position_;
}

glm::vec3 Entity::getRotation() const
{
	return rotation_;
}

glm::vec3 Entity::getScale() const
{
	return scale_;
}

void Entity::setPosition(glm::vec3 position)
{
	position_ = position;
}

void Entity::setRotation(glm::vec3 rotation)
{
	rotation_ = rotation;
}

void Entity::setScale(glm::vec3 scale)
{
	scale_ = scale;
}

