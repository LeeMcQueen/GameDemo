#pragma once

#include "ShaderProgram.h"
#include "Light.h"

class WaterShader : public ShaderProgram {

public:

	static const char *WATER_VERTEX_FILE;
	static const char *WATER_FRAGMENT_FILE;

	WaterShader();

	//变换矩阵加载 参数类型mat4
	void loadTransformationMatrix(glm::mat4 matrix);
	//投影矩阵加载 参数类型mat4
	void loadProjectionMatrix(glm::mat4 matrix);
	//观察矩阵加载 参数类型mat4
	void loadViewMatrix(glm::mat4 matrix);
	//加载纹理集合
	void loadconnectTextureUnits();
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
	GLuint Location_reflectionTexture;
	GLuint Location_refractionTexture;
	GLuint Location_dudvMap;
};