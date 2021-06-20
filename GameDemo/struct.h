#pragma once

#include <assimp/scene.h>
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

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

//inline SDL_Window* initWindow(int &windowWidth, int &windowHeight) {
//	SDL_Init(SDL_INIT_EVERYTHING);
//	SDL_GL_LoadLibrary(NULL);
//
//	//window
//	SDL_Window* window = SDL_CreateWindow("skin Animation",
//		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//		640, 480,
//		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
//
//
//	SDL_GLContext context = SDL_GL_CreateContext(window);
//
//	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
//	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
//	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
//	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
//
//
//	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
//
//	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
//	glViewport(0, 0, windowWidth, windowHeight);
//	glClearColor(1.0, 0.0, 0.4, 1.0);
//	glEnable(GL_DEPTH_TEST);
//	SDL_ShowWindow(window);
//	SDL_GL_SetSwapInterval(1);
//	return window;
//}
