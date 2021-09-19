#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtx\transform.hpp>

#include "ShaderProgram.h"
#include "Camera.h" 
#include "Maths.h"
#include "Light.h"

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
	//shader的使用假光源接口
	void loadUserFakeLighting(bool useFake);
	//shader天空颜色接口
	void loadSkyColour(float r, float g, float b);
	//切线平面
	void loadClipPlane(glm::vec4 plane);

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
	//假光源
	GLuint Location_useFakeLighting;
	//天空颜色
	GLuint Location_skyColour;
	//水面用的截切平面
	GLuint Location_plane;
};

