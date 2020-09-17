#include "Renderer.h"
#include "RawModel.h"
#include "TexturedModel.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void Renderer::Prepare()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1, 0, 0, 1);
}

//void Renderer::Render(RawModel model) {
void Renderer::Render(TexturedModel texturedModel) {

	//Get rawModel from textredModel
	RawModel& model = texturedModel.GetRawModel();

	glBindVertexArray(model.getVaoId());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturedModel.GetTextureModel().getID());

	//Draw function (MODEL triangles, Draw how many point, type, indices)
	/*glDrawArrays(GL_TRIANGLES, 0, model.getVertexCount());*/
	glDrawElements(GL_TRIANGLES, model.getVertexCount(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

