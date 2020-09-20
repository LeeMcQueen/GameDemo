#pragma once

#include <cmath>

#include "struct.h"

class Maths 
{
public:

	static float* createTransformationMatrix(vector3 position, vector3 rotation, float scale);

private:

	static void leftMultiMatrix(float* a, float* b);

	static void calculateRotationMatrix(float* res, vector3 rot, const char* order = "xyz");

	static void calculateTransformationMatrix(float* res, vector3 position);
};