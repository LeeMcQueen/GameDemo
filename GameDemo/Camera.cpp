#include "Camera.h"

#include <GLFW\glfw3.h>
#include <gtx\transform.hpp>

using namespace std;

Camera::Camera()
	/*:position(glm::vec3(0, 5, 0))*/
	:position(glm::vec3(0, 5, 0))
{}


Camera::~Camera()
{

}

void Camera::move()
{
	float speed = 0.25f;

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed *= 10;

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
		position.z -= speed;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
		position.z += speed;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
		position.x -= speed;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
		position.x += speed;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_UP) == GLFW_PRESS)
		position.y += speed;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DOWN) == GLFW_PRESS)
		position.y -= speed;

	cerr << "1.transformation x:" << position.x << endl;
	cerr << "2.transformation y:" << position.y << endl;
	cerr << "3.transformation z:" << position.z << endl;
}

glm::mat4 Camera::getViewMatrix() const
{
	/*return glm::lookAt(position, position + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));*/
	return glm::lookAt(position, position + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

glm::vec3 Camera::getPosition()
{
	return glm::vec3();
}

glm::vec3 Camera::getRotation()
{
	return glm::vec3();
}

void Camera::setPosition(glm::vec3 _position)
{

}

void Camera::setRotation(glm::vec3 _rotation)
{

}
