#include "Entity.h"

Entity::Entity(ModelTexture& texture_, vector3 position_, vector3 rotation_, float scale_, int atlasPosition_)
	:texture(texture_),
	position(position_), 
	rotation(rotation_),
	scale(scale_),
	atlasPosition(atlasPosition_)
{
	
}

Entity::~Entity()
{

}
