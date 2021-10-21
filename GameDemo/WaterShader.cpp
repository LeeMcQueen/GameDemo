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

void WaterShader::loadCameraPosition(glm::vec3 position){

	loadVector3(Location_cameraPosition, position);
}

void WaterShader::loadLightPosition(glm::vec3 lightPosition){

	loadVector3(Location_lightPosition, lightPosition);
}

void WaterShader::loadLightColor(glm::vec3 lightColor){

	loadVector3(Location_lightColor, lightColor);
}

void WaterShader::loadconnectTextureUnits(){

	loadInt(Location_reflectionTexture, 0);
	loadInt(Location_refractionTexture, 1);
	loadInt(Location_dudvMap, 2);
	loadInt(Location_normalMap, 3);
	loadInt(Location_depthMap, 4);
}

void WaterShader::loadMoveFactor(float factor){

	loadFloat(Location_moveFactor, factor);
}

void WaterShader::getAllUniformLocations() {

	Location_transformtionMatrix = getUniformLocation("transformationMatrix");
	Location_projectionMatrix = getUniformLocation("projectionMatrix");
	Location_viewMatrix = getUniformLocation("viewMatrix");
	Location_reflectionTexture = getUniformLocation("reflectionTexture");
	Location_refractionTexture = getUniformLocation("refractionTexture");
	Location_dudvMap = getUniformLocation("dudvMap");
	Location_normalMap = getUniformLocation("normalMap");
	Location_depthMap = getUniformLocation("depthMap");
	Location_moveFactor = getUniformLocation("moveFactor");
	Location_cameraPosition = getUniformLocation("cameraPosition");
	Location_lightColor = getUniformLocation("lightColor");
	Location_lightPosition = getUniformLocation("lightPosition");

}