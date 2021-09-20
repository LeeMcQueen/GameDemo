#include "WaterShader.h"

const char* WaterShader::WATER_VERTEX_FILE = "waterVertexShader.txt";
const char* WaterShader::WATER_FRAGMENT_FILE = "waterFragmentShader.txt";

WaterShader::WaterShader() : ShaderProgram(WATER_VERTEX_FILE, WATER_FRAGMENT_FILE) {

	//给传入shader的VBO进行命名
	bindAttributes();

	//Bind VAO attributes and link program
	glLinkProgram(programID);
	glValidateProgram(programID);

	//加载uniform到顶点shader
	getAllUniformLocations();
}

void WaterShader::bindAttributes() {

	bindAttribute(0, "position");
}


void WaterShader::loadTransformationMatrix(glm::mat4 matrix) {

	loadMatrix4(Location_transformtionMatrix, matrix);
}

void WaterShader::loadProjectionMatrix(glm::mat4 matrix) {

	loadMatrix4(Location_projectionMatrix, matrix);
}

void WaterShader::loadViewMatrix(glm::mat4 matrix) {

	loadMatrix4(Location_viewMatrix, matrix);
}

void WaterShader::getAllUniformLocations() {

	Location_transformtionMatrix = getUniformLocation("transformationMatrix");
	Location_projectionMatrix = getUniformLocation("projectionMatrix");
	Location_viewMatrix = getUniformLocation("viewMatrix");
}