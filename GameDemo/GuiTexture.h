#pragma once

#include <glm.hpp>

class GuiTexture {

public:
	GuiTexture(unsigned int texture, glm::vec2 position, glm::vec2 scale) :
		texture_(texture),
		position_(position),
		scale_(scale) {};

	unsigned int getTexture() { return texture_; }
	glm::vec2 getPosition() { return position_; }
	glm::vec2 getScale() { return scale_; }

private:

	unsigned int texture_;
	glm::vec2 position_;
	glm::vec2 scale_;
};