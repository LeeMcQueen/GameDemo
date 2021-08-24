#pragma once

#include "ShaderProgram.h"
#include "Light.h"

class WaterShader : public ShaderProgram {

public:

	WaterShader();

	void loadTransformationMatrix(glm::mat4 matrix);

	void loadProjectionMatrix(glm::mat4 matrix);

	void loadViewMatrix(glm::mat4 matrix);

	void loadLight(Light light);

};