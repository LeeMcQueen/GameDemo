#pragma once

#include <iostream>
#include <gtx\transform.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	//increase position&rotation into view(camera)Matrix
	void move();

	glm::mat4 getViewMatrix() const;

	//get camera position
	static glm::vec3 getPosition();

	//get camera rotation
	static glm::vec3 getRotation();

	//set camera position
	static void setPosition(glm::vec3 _position);

	//set camera rotation
	static void setRotation(glm::vec3 _rotation);

private:

	//camera position
	glm::vec3 position;

	//camera rotation
	static glm::vec3 rotation;
};

