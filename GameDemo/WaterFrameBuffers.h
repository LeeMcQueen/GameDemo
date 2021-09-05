#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class WaterFrameBuffers{

public:
	WaterFrameBuffers();
	~WaterFrameBuffers();

private:

	unsigned int createFrameBuffer();
};