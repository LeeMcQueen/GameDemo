#pragma once

struct vector3
{
	float x, y, z;
	vector3(float x_ = 0, float y_ = 0, float z_ = 0) :x(x_), y(y_), z(z_) {}

	vector3 operator-() const
	{
		vector3 returnValue;
		returnValue.x = -x;
		returnValue.y = -y;
		returnValue.z = -z;

		return returnValue;
	}
};