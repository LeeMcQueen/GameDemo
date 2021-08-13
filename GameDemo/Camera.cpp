#include <GLFW/glfw3.h>

#include "Camera.h"

using namespace std;

Camera::Camera(Player player)
	:position_(glm::vec3(0, 40, 50)),
	distanceFromPlayer_(50.0f),
	angleAroundPlayer_(0.0f)
{}

void Camera::move()
{
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		position_.z -= 1.2;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		position_.x += 1.2;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		position_.x -= 1.2;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		position_.z += 1.2;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		rotation_.y -= 5;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		rotation_.y += 5;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_UP) == GLFW_PRESS)
	{
		rotation_.x -= 5;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		rotation_.x += 5;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position_.y += 1.1;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		position_.y -= 1.1;
	}
}

void Camera::calclateZoom() {

	//float zoomLevel = 
}

glm::mat4 Camera::getViewMatrix() const {

	return glm::lookAt(position_, position_ + glm::vec3(0, -3.0f, -3.0f), glm::vec3(0, 1.0f, 0));
}

glm::vec3 Camera::getPosition() const {

	return position_;
}

glm::vec3 Camera::getRotation() const {

	return rotation_;
}
