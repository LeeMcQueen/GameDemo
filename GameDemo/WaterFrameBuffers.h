#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class WaterFrameBuffers{

public:
	WaterFrameBuffers();
	~WaterFrameBuffers();

	void unbindCurrentFrameBuffer();
	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void cleanUp();

	unsigned int getReflectionTexture() const { return reflectionTexture_; }
	unsigned int getRefractionTexture() const { return refractionTexture_; }
	unsigned int getRefractionDepthTexture() const { return refractionDepthTexture_; }

private:
	const int REFLECTION_WIDTH = 320;
	const int REFLECTION_HEIGHT = 180;
	const int REFRACTION_WIDTH = 720;
	const int REFRACTION_HEIGHT = 320;

	unsigned int reflectionFrameBuffer_;
	unsigned int reflectionTexture_;
	unsigned int reflectionDepthBuffer_;

	unsigned int refractionFrameBuffer_;
	unsigned int refractionTexture_;
	unsigned int refractionDepthTexture_;

	void initReflectionBuffer();
	void initRefractionBuffer();
	unsigned int createFrameBuffer();
	unsigned int createTextureAttachment(int width, int height);
	unsigned int createDepthTextureAttachment(int width, int height);
	unsigned int createDepthBufferAttachment(int width, int height);
	void bindFrameBuffer(unsigned int framebuffer, int width, int height);
};