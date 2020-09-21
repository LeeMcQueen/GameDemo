#include "Entity.h"

Entity::Entity(TexturedModel& model, vector3 position, vector3 rotation, float scale)
	:model_(model),
	position_(position),
	rotation_(rotation),
	scale_(scale)
{
	
}

Entity::~Entity()
{

}

void Entity::increasePosition(float x_, float y_, float z_)
{
	position_.x += x_;
	position_.y += y_;
	position_.z += z_;
}

void Entity::increaseRotation(float pitch_, float yaw_, float roll_)
{
	rotation_.x += pitch_;
	rotation_.y += yaw_;
	rotation_.z += roll_;
}

TexturedModel Entity::GetTexture() const
{
	return model_;
}

vector3 Entity::getPosition() const
{
	return position_;
}

vector3 Entity::getRotation() const
{
	return rotation_;
}

float Entity::getScale() const
{
	return scale_;
}

void Entity::setPosition(vector3 position)
{
	position_ = position;
}

void Entity::setRotation(vector3 rotation)
{
	rotation_ = rotation;
}

void Entity::setScale(float scale)
{
	scale_ = scale;
}

