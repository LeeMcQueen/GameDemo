#pragma once

#include <cmath>
#include <gtx\transform.hpp>

#include "struct.h"

class Maths 
{
public:

	static glm::mat4 createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

private:

	//static void leftMultiMatrix(float* a, float* b);

	//static void calculateRotationMatrix(float* res, vector3 rot, const char* order = "xyz");

	//static void calculateTransformationMatrix(float* res, vector3 position);
};