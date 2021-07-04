#include "Entity.h"

#include <iostream>

using namespace std;

Entity::Entity(const TexturedModel &model, const glm::vec3 position, const glm::vec3 rotation, const glm::vec3 scale)
	:model_(model),
	position_(position),
	rotation_(rotation),
	scale_(scale)
{

}

Entity::~Entity()
{

}

void Entity::increasePosition(const glm::vec3 position)
{
	position_ += position;
}

void Entity::increaseRotation(const glm::vec3 rotation)
{
	rotation_ += rotation;
}

TexturedModel Entity::GetModel() const
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

void Entity::setPosition(const glm::vec3 position)
{
	position_ = position;
}

void Entity::setRotation(const glm::vec3 rotation)
{
	rotation_ = rotation;
}

void Entity::setScale(const glm::vec3 scale)
{
	scale_ = scale;
}

