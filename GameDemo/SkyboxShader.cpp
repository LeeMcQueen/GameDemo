#include "SkyboxShader.h"

const char *SkyboxShader::SKYBOX_VERTEX_SHADER = "skyboxVertexShader.txt";
const char *SkyboxShader::SKYBOX_FRAGMENT_SHADER = "skyboxFragmentShader.txt";
float ROTATE_SPEED = 0.01f;

SkyboxShader::SkyboxShader() : ShaderProgram(SKYBOX_VERTEX_SHADER, SKYBOX_FRAGMENT_SHADER) {

	//给传入shader的VBO进行命名
	bindAttributes();

	//Bind VAO attributes and link program
	glLinkProgram(programID);
	glValidateProgram(programID);

	//加载uniform到顶点shader
	getAllUniformLocations();
}

void SkyboxShader::bindAttributes() {

	bindAttribute(0, "position");
}

void SkyboxShader::loadProjectionMatrix(glm::mat4 matrix) {

	loadMatrix4(Location_projectionMatrix, matrix);
}

void SkyboxShader::loadViewMatrix(Camera &camera) {

	//从camera类取得观察矩阵以后
	glm::mat4 matrix = camera.getViewMatrix();
	matrix[3] = glm::vec4{ 0.0, 0.0, 0.0, matrix[3][3] };
	rotation_ += ROTATE_SPEED;
	matrix = glm::rotate(matrix, glm::radians(rotation_), { 0, 1, 0 });
	loadMatrix4(Location_viewMatrix, matrix);
}

void SkyboxShader::getAllUniformLocations() {

	Location_projectionMatrix = getUniformLocation("projectionMatrix");
	Location_viewMatrix = getUniformLocation("viewMatrix");
}


