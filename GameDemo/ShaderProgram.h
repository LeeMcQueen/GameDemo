#pragma once
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	GLint programID;

	//bind VAO attribute id to shader variable name
	void bindAttribute(GLint attribute, const char* variableName);

	//interface. bind VAO attributes
	virtual void bindAttributes() = 0;

};

