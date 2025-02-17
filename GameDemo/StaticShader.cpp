﻿
#include "StaticShader.h"
#include "stdio.h"

//顶点着色器GLSL 片段着色器GLSL
const char* StaticShader::VERTEX_FILE = "vertexShader.txt";
const char* StaticShader::FRAGMENT_FILE = "fragmentShader.txt";

StaticShader::StaticShader(): ShaderProgram(VERTEX_FILE, FRAGMENT_FILE){

	//绑定VAO属性
	bindAttributes();

	//Bind VAO attributes and link program
	//绑定VAO属性链接程序
	glLinkProgram(programID);
	glValidateProgram(programID);

	//加载shader变量
	getAllUniformLocations();
}

//Bind VAO attributes
void StaticShader::bindAttributes(){

	bindAttribute(0, "position");
	bindAttribute(1, "textureCoords");
	bindAttribute(2, "normal");
}

void StaticShader::loadTransformationMatrix(glm::mat4 matrix){

	loadMatrix4(Location_tansformationMatrix, matrix);
}

void StaticShader::loadProjectionMatrix(glm::mat4 matrix){

	loadMatrix4(Location_projectionMatrix, matrix);
}

void StaticShader::loadViewMatrix(glm::mat4 matrix){

	loadMatrix4(Location_viewMatrix, matrix);
}

void StaticShader::loadLight(Light light){

	loadVector3(Location_lightColor, light.getColor());
	loadVector3(Location_lightPosition, light.getPosition());
}

void StaticShader::loadShineVariables(float shineDamper, float reflectivity){

	loadFloat(Location_shineDamper, shineDamper);
	loadFloat(Location_reflectivity, reflectivity);
}

void StaticShader::loadUserFakeLighting(bool useFake){

	loadBoolean(Location_useFakeLighting, useFake);
}

void StaticShader::loadSkyColour(float r, float g, float b){

	loadVector3(Location_skyColour, glm::vec3(r, g, b));
}

void StaticShader::loadClipPlane(glm::vec4 plane){

	loadVector4(Location_plane, plane);
}

//get all uniform location
void StaticShader::getAllUniformLocations()
{
	Location_tansformationMatrix = getUniformLocation("transformationMatrix");
	Location_projectionMatrix = getUniformLocation("projectionMatrix");
	Location_viewMatrix = getUniformLocation("viewMatrix");
	Location_lightColor = getUniformLocation("lightColor");
	Location_lightPosition = getUniformLocation("lightPosition");
	Location_shineDamper = getUniformLocation("shineDamper");
	Location_reflectivity = getUniformLocation("reflectivity");
	Location_useFakeLighting = getUniformLocation("useFakeLighting");
	Location_skyColour = getUniformLocation("skyColour");
	Location_plane = getUniformLocation("plane");
}
