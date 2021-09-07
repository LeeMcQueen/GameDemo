#pragma once

#include <glm.hpp>

class GuiTexture {

public:
	GuiTexture(unsigned int texture, glm::vec2 position, glm::vec2 scale) :
		texture_(texture) {};
	~GuiTexture();

	unsigned int getTexture() { return texture_; }

private:

	unsigned int texture_;
	glm::vec2 position_;
	glm::vec2 sacle_;
};