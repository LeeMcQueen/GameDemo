﻿#pragma once

#include <iostream>

#include "Camera.h"

//相机到主角远近移动速度
float DISTANCE_SPEED = 1.0f;
//相机升高降低的速度
float UPDOWN_SPEED = 1.0f;
//相机围绕主角旋转的速度
float TURN_SPEED = 5.0f;

Camera::Camera()
	:position_(glm::vec3(0.0f, 20.0f, 70.0f)),
	rotation_(glm::vec3(45.0f, 0.0f, 0.0f)),
	viewDirection_(glm::vec3(40.0f, 30.0f, 0.0f)),
	distanceFromPlayer_(50.0f),
	angleAroundPlayer_(0.0f) {};

void Camera::move(glm::vec3 playerPosition, glm::vec3 playerRotation)
{
	calclateZoom();
	calculatePitch();
	calculateAngleAoundPlayer();

	//玩家到相机的水平距离 distanceFromPlayer_ * cos(θ)/
	float horizontaDistance = calculateHorizontaDistance();
	//玩家到相机的垂直高度 distanceFromPlayer_ * sin(θ)
	float verticalDistance = calculateVerticalDistance();

	calculateCameraPosition(horizontaDistance, verticalDistance, playerPosition, playerRotation);
	//自由相机
	//calculateCameraPosition(horizontaDistance, verticalDistance, position_, playerRotation);
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

//相机到主角远近移动函数
void Camera::calclateZoom() {

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_O) == GLFW_PRESS) {

		distanceFromPlayer_ -= DISTANCE_SPEED;
	}
}

//相机高低调整函数
void Camera::calculatePitch() {

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS) {

		rotation_.x -= UPDOWN_SPEED;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

		rotation_.x += UPDOWN_SPEED;
	}
}

//相机围绕主角函数
void Camera::calculateAngleAoundPlayer() {

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS) {

		rotation_.y -= TURN_SPEED;
	}
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS) {

		rotation_.y += TURN_SPEED;
	}
}

float Camera::calculateHorizontaDistance(){

	return distanceFromPlayer_ * std::cos(glm::radians(rotation_.x));
}

float Camera::calculateVerticalDistance(){

	return distanceFromPlayer_ * std::sin(glm::radians(rotation_.x));
}

void Camera::calculateCameraPosition(float horizDistance, float verticDistance, glm::vec3 playerPosition, glm::vec3 playerRotation){

	//取消了使用player 暂时注释掉
	//float theta = player_.getRotation().z + rotation_.y;
	//float xOffset = horizDistance * std::sin(glm::radians(theta));
	//float zOffset = horizDistance * std::cos(glm::radians(theta));

	setPosition(playerPosition);
}

//观察矩阵函数
glm::mat4 Camera::getViewMatrix() const {

	/*方案2 相机函数*/
	//glm::mat4 view;
	//view = glm::rotate(view, glm::radians(rotation_.x), { 1, 0, 0 });
	//view = glm::rotate(view, glm::radians(rotation_.y), { 0, 1, 0 });
	//view = glm::translate(view, -position_);
	//return view;

	/*方案1 相机函数*/
	//相机位置  目标位置 相机上方向
	return glm::lookAt(position_ + viewDirection_, position_, glm::vec3(0, 1.0f, 0));
}

glm::vec3 Camera::getPosition() const {

	return position_;
}

glm::vec3 Camera::getRotation() const {

	return rotation_;
}
