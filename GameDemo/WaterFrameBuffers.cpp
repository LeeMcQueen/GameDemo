#include "WaterFrameBuffers.h"

WaterFrameBuffers::WaterFrameBuffers(){


}

WaterFrameBuffers::~WaterFrameBuffers()
{
}

unsigned int WaterFrameBuffers::createFrameBuffer(){

	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	return 0;
}
