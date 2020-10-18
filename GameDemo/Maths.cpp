#include <cmath>

#include "Maths.h"

glm::mat4 Maths::createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	//calculate translation Matrix
	glm::mat4 transformationMatrix = glm::translate(position);

	//calculate Rotation Matrix
	glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1, 0, 0))*
		glm::rotate(rotation.y, glm::vec3(0, 1, 0))*
		glm::rotate(rotation.z, glm::vec3(0, 0, 1));

	//calculate Scaling Matrix
	glm::mat4 scaleMatrix = glm::scale(scale);

	return transformationMatrix * rotationMatrix * scaleMatrix;
}

//Not use
glm::mat4 Maths::createViewMatrix(Camera & camera)
{
	glm::mat4 view;

	view = glm::rotate(view, glm::radians(camera.getRotation().x), { 1, 0, 0 });
	view = glm::rotate(view, glm::radians(camera.getRotation().y), { 0, 1, 0 });
	view = glm::translate(view, -camera.getPosition());

	return view;
}
