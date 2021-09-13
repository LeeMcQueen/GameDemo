#pragma once

#include "ShaderProgram.h"

class GuiShader : public ShaderProgram {

public:

	GuiShader();

	static const char *GUI_VERTEX_FILE;
	static const char *GUI_FRAGMENT_FILE;

	void loadTransformationMatrix(glm::mat4 matrix);

protected:

	void bindAttributes() override;

	void getAllUniformLocations() override;

private:

	GLuint Location_transformationMatrix;
};