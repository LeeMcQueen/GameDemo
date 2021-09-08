#pragma once

#include <cmath>
#include <gtx\transform.hpp>

#include "struct.h"
#include "camera.h"

class Maths 
{
public:

	static glm::mat4 createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	static glm::mat4 createTransformationMatrix(glm::vec2 &translation, glm::vec2 &scale);
};