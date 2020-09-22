#pragma once

#include "ShaderProgram.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtx\transform.hpp>

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

	void loadTransformationMatrix(glm::mat4 matrix);

protected:

	//bind VAO attribules
	void bindAttributes() override;

	//get all uniform locations
	void getAllUniformLocations() override;

private:

	//Location of transformation matrix in shader progam
	GLuint Location_tansformationMatrix;
};

