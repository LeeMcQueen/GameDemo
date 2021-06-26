#include "Camera.h"

#include <GLFW\glfw3.h>
#include <gtx\transform.hpp>

using namespace std;

Camera::Camera()
	:position(glm::vec3(0, 40, 50))		//Camera location
{}

void Camera::move()
{
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		position.z -= 0.2;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		position.x += 0.2;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		position.x -= 0.2;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		position.z += 0.2;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		rotation.y -= 5;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		rotation.y += 5;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_UP) == GLFW_PRESS)
	{
		rotation.x -= 5;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		rotation.x += 5;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position.y += 0.1;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		position.y -= 0.1;
	}

	//cerr << "1.transformation x:" << position.x << endl;
	//cerr << "2.transformation y:" << position.y << endl;
	//cerr << "3.transformation z:" << position.z << endl;
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + glm::vec3(0, -3.0f, -3.0f), glm::vec3(0, 1.0f, 0));
}

glm::vec3 Camera::getPosition() const
{
	return position;
}

glm::vec3 Camera::getRotation() const
{
	return rotation;
}
