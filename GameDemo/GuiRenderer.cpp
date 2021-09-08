#include "GuiRenderer.h"

RawModel GuiRenderer::rawModel_;
GuiShader GuiRenderer::guiShader_;

GuiRenderer::GuiRenderer(Loader loader){

	std::vector<float> positions = { -1, 1, -1, -1, 1, 1, 1, -1 };
	rawModel_ = loader.loadToVAO(positions, 2);
}

void GuiRenderer::render(std::vector<GuiTexture> guiTextures) {

	guiShader_.start();
	glBindVertexArray(rawModel_.getVaoId());
	glEnableVertexAttribArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	for (GuiTexture &guiTexture : guiTextures) {

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guiTexture.getTexture());
		glm::mat4 transformMatrix = Maths::createTransformationMatrix(guiTexture.getPosition(), guiTexture.getScale());
		guiShader_.loadTransformationMatrix(transformMatrix);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, rawModel_.getVertexCount());
	}
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	guiShader_.stop();
}

void GuiRenderer::cleanUp() {

	guiShader_.stop();
}
