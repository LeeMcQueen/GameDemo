#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ShadowFrameBuffer {

public:
	ShadowFrameBuffer();

	void bindShadowFrameBuffer();
	void unbindShadowFrameBuffer();
	void cleanUp();

	unsigned int getShadowMap() const { return shadowMap_; }

private:
	const int SHADOW_WIDTH = 1280;
	const int SHADOW_HEIGHT = 720;

	unsigned int shadowMap_;
	unsigned int shadowMapFBO_;

	void initialiseFrameBuffer();
	unsigned int createFrameBuffer();
	unsigned int createDepthBufferAttachment(int width, int height);

	void bindFrameBuffer(unsigned int framebuffer, int width, int height);
};