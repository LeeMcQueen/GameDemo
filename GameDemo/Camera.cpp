#include <GLFW/glfw3.h>

#include "Camera.h"

//相机到主角远近移动速度
float DISTANCE_SPEED = 1.0f;
//相机升高降低的速度
float UPDOWN_SPEED = 1.0f;
//相机围绕主角旋转的速度
float TURN_SPEED = 1.0f;

Camera::Camera(Player player)
	:position_(glm::vec3(0, 20, 50)),
	rotation_(glm::vec3(45.0f, 0.0f, 0.0f)),
	distanceFromPlayer_(50.0f),
	angleAroundPlayer_(0.0f) {}

void Camera::move()
{
	//if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	//{
	//	position_.z -= 1.2;
	//}
	//if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	//{
	//	position_.x += 1.2;
	//}
	//if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	//{
	//	position_.x -= 1.2;
	//}
	//if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	//{
	//	position_.z += 1.2;
	//}

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

	//if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
	//{
	//	position_.y += 1.1;
	//}
	//if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	//{
	//	position_.y -= 1.1;
	//}
}

void Camera::calclateZoom() {

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_O) == GLFW_PRESS) {

		float zoomLevel = DISTANCE_SPEED;
		distanceFromPlayer_ -= zoomLevel;
	}
}

void Camera::calculatePitch() {

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS) {

		float pitchChange = UPDOWN_SPEED;
		rotation_.x += pitchChange;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

		float pitchChange = UPDOWN_SPEED;
		rotation_.x -= pitchChange;
	}
}

//相机围绕主角函数
void Camera::calculateAngleAoundPlayer() {

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS) {

		float pitchChange = TURN_SPEED;
		rotation_.y += pitchChange;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS) {

		float pitchChange = TURN_SPEED;
		rotation_.y -= pitchChange;
	}
}

//观察矩阵函数
glm::mat4 Camera::getViewMatrix() const {

	/*方案2 相机函数*/
	glm::mat4 view;

	view = glm::rotate(view, glm::radians(rotation_.x), { 1, 0, 0 });
	view = glm::rotate(view, glm::radians(rotation_.y), { 0, 1, 0 });
	view = glm::translate(view, -position_);

	return view;
	/*方案1 相机函数*/
	//return glm::lookAt(position_, position_ + glm::vec3(0, -3.0f, -3.0f), glm::vec3(0, 1.0f, 0));
}

glm::vec3 Camera::getPosition() const {

	return position_;
}

glm::vec3 Camera::getRotation() const {

	return rotation_;
}
