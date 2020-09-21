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

	getAllUniformLocations();
}

//Bind VAO attributes
void StaticShader::bindAttributes()
{
	bindAttribute(0, "position");
	bindAttribute(1, "textureCoords");
}

void StaticShader::loadTransformationMatrix(const float * matrix)
{
	loadMatrix4(Location_tansformationMatrix, matrix);
}

//get all uniform location
void StaticShader::getAllUniformLocations()
{
	Location_tansformationMatrix = getUniformLocation("transformationMatrix");
}
