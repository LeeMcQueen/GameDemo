#include "WaterShader.h"

const char* WaterShader::WATER_VERTEX_FILE = "terrainVertexShader.txt";
const char* WaterShader::WATER_FRAGMENT_FILE = "terrainFragmentShader.txt";

WaterShader::WaterShader()
	: ShaderProgram(WATER_VERTEX_FILE, WATER_FRAGMENT_FILE)
{

}

void WaterShader::loadTransformationMatrix(glm::mat4 matrix){

}

void WaterShader::loadProjectionMatrix(glm::mat4 matrix){

}

void WaterShader::loadViewMatrix(glm::mat4 matrix){

}

void WaterShader::loadLight(Light light){

}

void WaterShader::bindAttributes()
{
}

void WaterShader::getAllUniformLocations()
{
}
