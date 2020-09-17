#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ModelTexture
{
public:
	//Constructor
	ModelTexture(GLuint id);
	//Destructor
	~ModelTexture();
	//Get texture id
	GLuint getID() const;

private:
	GLuint textureID_;
};

