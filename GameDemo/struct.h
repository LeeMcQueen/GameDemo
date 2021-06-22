#pragma once

#include <iostream>
#include <assimp/scene.h>
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef unsigned char byte;

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

inline glm::mat4 assimpToGlmMatrix(aiMatrix4x4 assimpMat4)
{
	glm::mat4 tempMatrix;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			tempMatrix[x][y] = assimpMat4[y][x];
		}
	}
	return tempMatrix;
}

inline glm::vec3 assimpToGlmVec3(aiVector3D vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::quat assimpToGlmQuat(aiQuaternion quat)
{
	glm::quat tempquat;
	tempquat.x = quat.x;
	tempquat.y = quat.y;
	tempquat.z = quat.z;
	tempquat.w = quat.w;

	return tempquat;
}

inline unsigned int createShader(const char *vertexStr, const char *fragmentStr) {
	int success;
	char info_log[512];

	//建立shader程序
	unsigned int program = glCreateProgram();
	unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vertexStr, 0);
	glCompileShader(vShader);
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vShader, 512, 0, info_log);
		std::cout << "vertex shader compilation failed! \n" << info_log << std::endl;
	}
	glShaderSource(fShader, 1, &fragmentStr, 0);
	glCompileShader(fShader);
	//打印链接错误(如果有的话)
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fShader, 512, 0, info_log);
		std::cout << "fragment shader compilation failed! \n" << info_log << std::endl;
	}

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, 0, info_log);
		std::cout << "program link failed! \n" << info_log << std::endl;
	}

	glDetachShader(program, vShader);
	glDeleteShader(vShader);
	glDetachShader(program, fShader);
	glDeleteShader(fShader);

	return program;
}
