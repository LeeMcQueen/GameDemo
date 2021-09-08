#include "GuiShader.h"

const char *GuiShader::GUI_VERTEX_FILE = "guiVertexShader.txt";
const char *GuiShader::GUI_FRAGMENT_FILE = "guiFragmentShader.txt";

GuiShader::GuiShader():ShaderProgram(GUI_VERTEX_FILE, GUI_FRAGMENT_FILE) {

	bindAttributes();

	glLinkProgram(programID);
	glValidateProgram(programID);

	getAllUniformLocations();
}

void GuiShader::bindAttributes() {

	bindAttribute(0, "position");
}

void GuiShader::loadTransformationMatrix(glm::mat4 matrix) {

	loadMatrix4(Location_transformationMatrix, matrix);
}

void GuiShader::getAllUniformLocations() {

	Location_transformationMatrix = getUniformLocation("transformationMatrix");
}