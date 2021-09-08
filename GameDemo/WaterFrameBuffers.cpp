#include "WaterFrameBuffers.h"

WaterFrameBuffers::WaterFrameBuffers(){

	initReflectionBuffer();
	initRefractionBuffer();
}

WaterFrameBuffers::~WaterFrameBuffers(){}

void WaterFrameBuffers::initReflectionBuffer(){

	reflectionFrameBuffer_ = createFrameBuffer();
	reflectionTexture_ = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	reflectionDepthBuffer_ = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	unbindCurrentFrameBuffer();
}

void WaterFrameBuffers::initRefractionBuffer(){

	refractionFrameBuffer_ = createFrameBuffer();
	refractionTexture_ = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	refractionDepthTexture_ = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	unbindCurrentFrameBuffer();
}


unsigned int WaterFrameBuffers::createFrameBuffer(){

	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return frameBuffer;
}

unsigned int WaterFrameBuffers::createTextureAttachment(int width, int height){

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	return texture;
}

unsigned int WaterFrameBuffers::createDepthTextureAttachment(int width, int height){

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

	return texture;
}

unsigned int WaterFrameBuffers::createDepthBufferAttachment(int width, int height){

	unsigned int depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}

void WaterFrameBuffers::bindFrameBuffer(unsigned int framebuffer, int width, int height){

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glViewport(0, 0 , width, height);
}


void WaterFrameBuffers::unbindCurrentFrameBuffer() {

	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, 720, 320);
}

void WaterFrameBuffers::bindReflectionFrameBuffer() {

	bindFrameBuffer(reflectionFrameBuffer_, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void WaterFrameBuffers::bindRefractionFrameBuffer() {

	bindFrameBuffer(refractionFrameBuffer_, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterFrameBuffers::cleanUp(){

}
