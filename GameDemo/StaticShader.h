#pragma once

#include "ShaderProgram.h"
#include "Camera.h" 
#include "Maths.h"
#include "Light.h"

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

	//load transformationMatrix to shader program
	void loadTransformationMatrix(glm::mat4 matrix);

	//load projectionMatrix to shader program
	void loadProjectionMatrix(glm::mat4 matrix);

	//load viewMatrix to shader program
	void loadViewMatrix(glm::mat4 matrix);

	//load light to shader program
	void loadLight(Light light);

	//load ShineDamper reflectivity to program
	void loadShineVariables(float shineDamper, float reflectivity);

	//shaderìIégópòÔåıåπê⁄å˚
	void loadUserFakeLighting(bool useFake);

protected:

	//bind VAO attribules
	void bindAttributes() override;

	//get all uniform locations
	void getAllUniformLocations() override;

private:

	//Location of transformation matrix in shader progam
	GLuint Location_tansformationMatrix;
	//location of projection matrix in shader program
	GLuint Location_projectionMatrix;
	//location of view matrix in shader program
	GLuint Location_viewMatrix;
	//location of lightColor in shader program
	GLuint Location_lightColor;
	//location of lightPosition in shader program
	GLuint Location_lightPosition;
	//location of shineDamper
	GLuint Location_shineDamper;
	//Location of reflectivity
	GLuint Location_reflectivity;
	//òÔåıåπ
	GLuint Location_useFakeLighting;
};

