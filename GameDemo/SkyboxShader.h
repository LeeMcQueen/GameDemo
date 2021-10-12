#pragma once

#include "ShaderProgram.h"
#include "Camera.h"

class SkyboxShader : public ShaderProgram {

public:

	static const char *SKYBOX_VERTEX_SHADER;
	static const char *SKYBOX_FRAGMENT_SHADER;

	SkyboxShader();

	void loadProjectionMatrix(glm::mat4 matrix);
	void loadViewMatrix(Camera &camera);

protected:

	void bindAttributes() override;
	void getAllUniformLocations() override;

private:

	GLuint Location_projectionMatrix;
	GLuint Location_viewMatrix;
	float rotation_ = 0.0f;
};