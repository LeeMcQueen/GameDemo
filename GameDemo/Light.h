#pragma once

#include <glm.hpp>

class Light
{
public:

	Light(const glm::vec3& position, const glm::vec3& color)
		:position_(position), color_(color) {}

	const glm::vec3& getPosition() const { return position_; }
	const glm::vec3& getColor() const { return color_; }

	void setColor(const glm::vec3 & color) { color_ = color; }

private:

	glm::vec3 position_;
	glm::vec3 color_;
};

