#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ModelTexture
{
public:
	//Constructor
	ModelTexture(GLuint id, bool transparent = false, bool fakeLighting = false)
		:textureID_(id) 
		,shineDamper_(100.0f) 
		,reflectivity_(1.0f)
		,hasTransParency_(transparent)
		,fakeLighting_(fakeLighting)
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
	void setShineDamer(float shineDamper)
	{
		ModelTexture::shineDamper_ = shineDamper;
	}
	//Get reflectivit
	float getReflectivity() const 
	{
		return reflectivity_;
	}
	//set reflectivity
	void setReflectivity(float reflectivity)
	{
		ModelTexture::reflectivity_ = reflectivity;
	}
	//get 背面剔除接口
	bool getTransparency() const {
		return hasTransParency_;
	}
	//set 背面剔除接口
	void setTransparent(bool hasTransparecy)
	{
		ModelTexture::hasTransParency_ = hasTransparecy;
	}
	bool getFakeLighting() const 
	{
		return fakeLighting_;
	}
	void setFakeLighting(bool fakeLighting)
	{
		ModelTexture::fakeLighting_ = fakeLighting;
	}

private:

	GLuint textureID_;

	float shineDamper_;

	float reflectivity_;

	//是否开始背面剔除判断
	bool hasTransParency_;
	//开启向上的伪装法线
	bool fakeLighting_;
};

