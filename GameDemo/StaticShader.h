#pragma once

#include "ShaderProgram.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

class StaticShader : public ShaderProgram
{
public:
	//Constructory
	StaticShader();

	//VertexShader file name
	static const char* VERTEX_FILE;

	//FragmentShader file name
	static const char* FRAGMENT_FILE;

private:

protected:

	void bindAttributes() override;
};

