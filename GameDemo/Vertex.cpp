#include "Vertex.h"

Vertex::Vertex() {}

Vertex::~Vertex() {}

void Vertex::setPosition(const glm::vec3 position)
{
	position_ = position;
}
glm::vec3 Vertex::getPosition() const
{
	return position_;
};
