#include "ShadowFrameBuffer.h"

ShadowFrameBuffer::ShadowFrameBuffer(){

	initialiseFrameBuffer();
}

void ShadowFrameBuffer::initialiseFrameBuffer() {

	shadowMapFBO_ = createFrameBuffer();
	shadowMap_ = createDepthBufferAttachment(SHADOW_WIDTH, SHADOW_HEIGHT);
	unbindCurrentFrameBuffer();
}

void ShadowFrameBuffer::bindShadowFrameBuffer(){

	bindFrameBuffer(shadowMapFBO_, SHADOW_WIDTH, SHADOW_HEIGHT);
}

void ShadowFrameBuffer::unbindCurrentFrameBuffer(){

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
}

void ShadowFrameBuffer::cleanUp(){

	glDeleteFramebuffers(1, &shadowMapFBO_);
	glDeleteTextures(1, &shadowMap_);
}

unsigned int ShadowFrameBuffer::createDepthBufferAttachment(int width, int height){

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

	return texture;
}

unsigned int ShadowFrameBuffer::createFrameBuffer(){

	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_NONE);

	return frameBuffer;
}

void ShadowFrameBuffer::bindFrameBuffer(unsigned int framebuffer, int width, int height){

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, width, height);
}
