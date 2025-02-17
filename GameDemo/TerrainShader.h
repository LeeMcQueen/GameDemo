﻿#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtx\transform.hpp>

#include "ShaderProgram.h"
#include "Camera.h" 
#include "Maths.h"
#include "Light.h"

class TerrainShader : public ShaderProgram{

public:

	TerrainShader();

	//VertexShader file name
	static const char *VERTEX_FILE;
	//FragmentShader file name
	static const char *FRAGMENT_FILE;

	//load transformationMatrix to shader program
	void loadTransformationMatrix(glm::mat4 matrix);
	//load projectionMatrix to shader program
	void loadProjectionMatrix(glm::mat4 matrix);
	//加载纹理集合
	void loadconnectTextureUnits();
	//load viewMatrix to shader program
	void loadViewMatrix(glm::mat4 matrix);
	//阴影空间矩阵
	void loadLightViewMatrix(glm::mat4 matrix);
	void loadLightOrthoMatrix(glm::mat4 matrix);
	//load light to shader program
	void loadLight(Light light);
	//load ShineDamper reflectivity to program
	void loadShineVariables(float shineDamper, float reflectivity);
	//切线平面
	void loadCilpPlane(glm::vec4 plane);

protected:

	//bind VAO attribules
	void bindAttributes() override;
	//get all uniform locations
	void getAllUniformLocations() override;

private:

	//Location of transformation matrix in shader progam
	GLuint Location_transformationMatrix;
	//location of projection matrix in shader program
	GLuint Location_projectionMatrix;
	//location of view matrix in shader program
	GLuint Location_viewMatrix;
	//location of lightColor in shader program
	GLuint Location_lightColor;
	//location of lightPosition in shader program
	GLuint Location_lightPosition;
	//location of shineDamper;
	GLuint Location_shineDamper;
	//Location of reflectivity
	GLuint Location_reflectivity;
	//Location of backgroundTexture
	GLuint Location_backgroundTexture;
	//Location of r
	GLuint Location_rTexture;
	//Location of g
	GLuint Location_gTexture;
	//Location of b
	GLuint Location_bTexture;
	//Location of blendMap
	GLuint Location_blendMap;
	//阴影纹理
	GLuint Location_shadowMap;
	//水面剪切平面uniform
	GLuint Location_plane;
	GLuint Location_lightViewMatrix;
	GLuint Location_lightOrthoMatrix;
};

