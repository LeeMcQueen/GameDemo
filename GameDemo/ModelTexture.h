#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ModelTexture
{
public:
	//Constructor
	ModelTexture(GLuint id) 
		:textureID_(id) 
		,shineDamper_(0.0f) 
		,reflectivity_(0.0f)
	{};

	//Destructor
	~ModelTexture() {};

	//Get texture id
	GLuint getID() const 
	{
		return textureID_;
	}
	//Get shineDamper
	float getShineDamer() const
	{
		return shineDamper_;
	}
	//Set shineDamper
	void setShineDamer(float shineDamer_)
	{
		ModelTexture::shineDamper_ = shineDamper_;
	}
	//Get reflectivit
	float getReflectivity() const 
	{
		return reflectivity_;
	}
	//set reflectivity
	void setReflectivity(float reflectivity_) 
	{
		ModelTexture::reflectivity_ = reflectivity_;
	}

private:

	GLuint textureID_;

	float shineDamper_;

	float reflectivity_;
};

