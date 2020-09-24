#include "StaticShader.h"

#include "stdio.h"

//vertexShader&fragmentShader file name
const char* StaticShader::VERTEX_FILE = "vertexShader.txt";
const char* StaticShader::FRAGMENT_FILE = "fragmentShader.txt";

StaticShader::StaticShader(): ShaderProgram(VERTEX_FILE, FRAGMENT_FILE)
{
	//bindAttribute
	bindAttributes();

	//Bind VAO attributes and link program
	glLinkProgram(programID);
	glValidateProgram(programID);

	//add uniform into vertexShader
	getAllUniformLocations();
}

//Bind VAO attributes
void StaticShader::bindAttributes()
{
	bindAttribute(0, "position");
	bindAttribute(1, "textureCoords");
}

void StaticShader::loadTransformationMatrix(glm::mat4 matrix)
{
	loadMatrix4(Location_tansformationMatrix, matrix);
}

void StaticShader::loadProjectionMatrix(glm::mat4 matrix)
{
	loadMatrix4(Location_projectionMatrix, matrix);
}

//void StaticShader::loadViewMatrix(Camera & camera)
//{
//	glm::mat4 viewMatrix= Maths::createViewMatrix(camera);
//	loadMatrix4(Location_viewMatrix, viewMatrix);
//}

void StaticShader::loadViewMatrix(glm::mat4 matrix)
{
	loadMatrix4(Location_viewMatrix, matrix);
}

//get all uniform location
void StaticShader::getAllUniformLocations()
{
	Location_tansformationMatrix = getUniformLocation("transformationMatrix");
	Location_projectionMatrix = getUniformLocation("projectionMatrix");
	Location_viewMatrix = getUniformLocation("viewMatrix");
}
