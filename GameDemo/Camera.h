#pragma once

#include <iostream>
#include <gtx\transform.hpp>

class Camera
{
public:
	Camera();

	//increase position&rotation into view(camera)Matrix
	void move();

	glm::mat4 getViewMatrix() const;

	//get camera position
	glm::vec3 getPosition() const;

	//get camera rotation
	glm::vec3 getRotation() const;

private:

	//camera position
	glm::vec3 position;
	//camera rotation
	glm::vec3 rotation;
};

