#pragma once

#include "ShaderProgram.h"
#include "Light.h"

class WaterShader : public ShaderProgram {

public:

	WaterShader();

	static const char *WATER_VERTEX_FILE;
	static const char *WATER_FRAGMENT_FILE;

	//变换矩阵加载 参数类型mat4
	void loadTransformationMatrix(glm::mat4 matrix);
	//投影矩阵加载 参数类型mat4
	void loadProjectionMatrix(glm::mat4 matrix);
	//观察矩阵加载 参数类型mat4
	void loadViewMatrix(glm::mat4 matrix);
	//光线信息加载 参数 1:vec3光线颜色 2:vec3光线位置
	void loadLight(Light light);
protected:

	//继承 给传入shader的VBO进行命名
	void bindAttributes() override;
	//继承 加载uniform 参数类型char
	void getAllUniformLocations() override;
private:

	//变换矩阵传递参数
	GLuint Location_transformtionMatrix;
	//投影矩阵传递参数
	GLuint Location_projectionMatrix;
	//观察矩阵传递参数
	GLuint Location_viewMatrix;
	////光线颜色传递参数
	//GLuint Location_lightColor;
	////光线位置传递参数
	//GLuint Location_lightPosition;
};