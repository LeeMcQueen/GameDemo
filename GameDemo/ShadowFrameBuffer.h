#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ShadowFrameBuffer {

public:
	ShadowFrameBuffer();




private:
	const int SHADOW_WIDTH = 1280;
	const int SHADOW_HEIGHT = 720;

};