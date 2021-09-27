#pragma once

#include "RawModel.h"
#include "Camera.h"
#include "Loader.h"
#include "SkyboxShader.h"

#include <gtc/matrix_transform.hpp>
#include <vector>

class SkyboxRenderer {

public:

	SkyboxRenderer(SkyboxShader &skyboxShader, Loader &loader, glm::mat4 projectionMatrix);
	void render(Camera &camera, const glm::vec3 &fogColor);

private:

	RawModel cube_;
	SkyboxShader skyboxShader_;

	unsigned int textureID_;
	unsigned int textureNightID_;

	void bindTextures();
};
