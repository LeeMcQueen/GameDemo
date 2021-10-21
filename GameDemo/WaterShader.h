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
	//水面移动时间参数
	void loadMoveFactor(float factor);
	//加载相机位置方法
	void loadCameraPosition(glm::vec3 position);
	//加载光线位置信息
	void loadLightPosition(glm::vec3 lightPosition);
	//加载光线颜色信息
	void loadLightColor(glm::vec3 lightColor);

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
	GLuint Location_normalMap;
	GLuint Location_moveFactor;
	GLuint Location_cameraPosition;
	//光
	GLuint Location_lightColor;
	GLuint Location_lightPosition;
};