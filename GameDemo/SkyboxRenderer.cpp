#include "SkyboxRenderer.h"

namespace {
	//ìVãÛ·¥ëÂè¨
	float SIZE = 500.0f;

	std::vector<float> VERTICES = {
		-SIZE,  SIZE, -SIZE,
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		-SIZE,  SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE,  SIZE
	};

	std::vector<std::string> TEXTURE = {
		"res/right.png",
		"res/left.png",
		"res/top.png",
		"res/bottom.png",
		"res/back.png",
		"res/front.png"
	};
}

SkyboxRenderer::SkyboxRenderer(SkyboxShader &skyboxShader, Loader &loader, glm::mat4 projectionMatrix) :
	cube_(loader.loadToVAO(VERTICES, 3)),
	textureID_(loader.loadCubeMap(TEXTURE)),
	skyboxShader_(skyboxShader) {

	skyboxShader_.start();
	skyboxShader_.loadProjectionMatrix(projectionMatrix);
	skyboxShader_.stop();
}

void SkyboxRenderer::render(Camera &camera, const glm::vec3 & fogColor) {

	skyboxShader_.start();
	skyboxShader_.loadViewMatrix(camera);
	glBindVertexArray(cube_.getVaoId());
	glEnableVertexAttribArray(0);

	bindTextures();

	glDrawArrays(GL_TRIANGLES, 0, cube_.getVertexCount());
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	skyboxShader_.stop();
}

void SkyboxRenderer::bindTextures() {

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID_);
}

