#include "ModelTexture.h"


//Constructor
ModelTexture::ModelTexture(GLuint id)
	:textureID_(id)
{}


ModelTexture::~ModelTexture()
{
	//glDeleteTextures(1, &textureID_);
}

//get texture id
GLuint ModelTexture::getID() const
{
	return textureID_;
}