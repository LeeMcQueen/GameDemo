#pragma once

#include "ShaderProgram.h"
#include "Light.h"

class WaterShader : public ShaderProgram {

public:

	WaterShader();

	static const char *WATER_VERTEX_FILE;

	static const char *WATER_FRAGMENT_FILE;

	void loadTransformationMatrix(glm::mat4 matrix);
	void loadProjectionMatrix(glm::mat4 matrix);
	void loadViewMatrix(glm::mat4 matrix);
	void loadLight(Light light);

protected:

	void bindAttributes() override;
	void getAllUniformLocations() override;

private:

	//变换矩阵传递参数
	GLuint Location_transformtionMatrix;
	//投影矩阵传递参数
	GLuint Location_projectionMatrix;
	//观察矩阵传递参数
	GLuint Location_viewMatrix;
	//光线颜色传递参数
	GLuint Location_lightColor;
	//光线位置传递参数
	GLuint Location_lightPosition;
};