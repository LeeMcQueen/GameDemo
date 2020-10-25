#pragma once

#include <assimp/scene.h>
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

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

inline glm::mat4 assimpToGlmMatrix(aiMatrix4x4 mat)
{
	glm::mat4 tempMatrix;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			tempMatrix[x][y] = mat[y][x];
		}
	}
	return tempMatrix;
}

inline glm::vec3 assimpToClmVec3(aiVector3D vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat assimpToGlmQuat(aiQuaternion quat)
{
	glm::quat q;
	q.x = quat.x;
	q.y = quat.y;
	q.z = quat.z;
	q.w = quat.w;

	return q;
}