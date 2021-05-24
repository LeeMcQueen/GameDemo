#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

ShaderProgram::ShaderProgram(const char* VertexFile, const char* FragmentFile){
	//load vertexShader
	vertexShaderID = ShaderProgram::loadShader(VertexFile, GL_VERTEX_SHADER);
	//load fragmentShader
	fragmentShaderID = ShaderProgram::loadShader(FragmentFile, GL_FRAGMENT_SHADER);

	//bulid shaderprogram
	programID = glCreateProgram();

	//vertexShader
	glAttachShader(programID, vertexShaderID);

	//fragmentShader
	glAttachShader(programID, fragmentShaderID);
}

void ShaderProgram::bindAttribute(GLint attribute, const char* variableName){
	glBindAttribLocation(programID, attribute, variableName);

	//cerr << "1." << programID << endl;
	//cerr << "2." << attribute << endl;
	//cerr << "3." <<variableName << endl;
}

ShaderProgram::~ShaderProgram(){
	stop();
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(programID);
}

void ShaderProgram::loadFloat(GLuint location, float value){
	glUniform1i(location, value);
}

void ShaderProgram::loadVector3(GLuint location, glm::vec3 vec){
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void ShaderProgram::loadBoolean(GLuint location, bool value){
	//true = 1 , false = 0
	glUniform1f(location, value ? 1.0 : 0.0);
}

void ShaderProgram::loadMatrix4(GLuint location, glm::mat4 value){
	//check
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

//uniform function
GLuint ShaderProgram::getUniformLocation(const char* uniformName){
	//1.check program 2.location name
	return glGetUniformLocation(programID, uniformName);
}

GLint ShaderProgram::loadShader(const char* fileName, int type){
	//Load shaderSource from file
	ifstream file(fileName);

	//Check file open
	if (!file.is_open())
	{
		cerr << "ERROR: [ShaderProgram::loadShader] Cannot load shader source" << fileName << "!" << endl;
		exit(-1);
	}

	string shaderSource = "";
	string line;

	//Line add to shaderSource
	while (getline(file, line))
	{
		shaderSource += line + "\n";
	}

	//Compile shader
	GLint shaderID = glCreateShader(type);
	const char* shaderSourceAdd = shaderSource.c_str();
	glShaderSource(shaderID, 1, &shaderSourceAdd, NULL);
	glCompileShader(shaderID);

	//Check comile status
	GLint status[1];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, status);

	//ERROR LOG
	if (status[0] == GL_FALSE)
	{
		GLint logLength[1];
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, logLength);
		char* shaderInfo = new char[logLength[0] + 1];
		glGetShaderInfoLog(shaderID, logLength[0], NULL, shaderInfo);

		cerr << "ERROR: [ShaderProgram::loadShader] Cannot compile shader " << fileName << "!" << endl;
		cerr << shaderInfo << endl;

		delete[] shaderInfo;
		glDeleteShader(shaderID);

		exit(-1);
	}

	cerr << "shaderSource:" << shaderSource << "End" << endl;

	return shaderID;
}
