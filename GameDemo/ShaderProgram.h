#pragma once
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtx\transform.hpp>

#include "struct.h"

class ShaderProgram
{
public:
	//Constructor
	ShaderProgram(const char* vetexFile, const char* fragmentFile);
	//Destructor
	virtual~ ShaderProgram();

	inline void start() { glUseProgram(programID); }
	inline void stop() { glUseProgram(0); }

private:

	GLint vertexShaderID;
	GLint fragmentShaderID;

	GLint loadShader(const char* fileName, int type);

protected:

	//Shader program id
	GLint programID;

	//interface get all uniform location
	virtual void getAllUniformLocations() = 0;
	//interface. bind VAO attributes
	virtual void bindAttributes() = 0;
	//get Uniform Location in shader program by name
	GLuint getUniformLocation(const char *uniformName);
	//bind VAO attribute id to shader variable name
	void bindAttribute(GLint attribute, const char *variableName);
	//load float into uniform by location
	void loadFloat(GLuint location, float value);
	//加载正数(int)的uniform
	void loadInt(GLuint location, int value);
	//load 3D vector into uniform by location
	void loadVector3(GLuint location, glm::vec3 vec);
	//加载vec4到shader的uniform
	void loadVector4(GLuint location, glm::vec4 vec);
	//load bool into uniform by location
	void loadBoolean(GLuint location, bool value);
	//load matrix into unifrom by location
	void loadMatrix4(GLuint location, glm::mat4 value);
};

