#pragma once

#include <iostream>

#include "Player.h"

const float RUN_SPEED = 1.0f;
const float TURN_SPEED = 5.0f;

Player::Player(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) :
	position_(position),
	rotation_(rotation),
	scale_(scale) {};

void Player::move(Terrain &terrain) {

	//键盘控制
	checkInputs();
	
	//TODO调用真正游戏运行时间
	rotation(0.0f, 0.0f, -currentTurnSpeed_ * 0.5f);
	
	//sin(θ) = x/distance cos(θ) = z/distance
	//TODO调用真正游戏运行时间
	float distance = currentSpeed_ * 0.5f;
	float deltaX = distance * std::sin(glm::radians(getRotation().z));
	float deltaZ = distance * std::cos(glm::radians(getRotation().z));

	float terrainHeight = terrain.getHeightOfTerrain(position_.x, position_.z);

	translate(-deltaX, terrainHeight + 9.5f, -deltaZ);
}

void Player::checkInputs() {
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) {
		currentSpeed_ = -RUN_SPEED;
	}
	else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
		currentSpeed_ = RUN_SPEED;
	}
	else {
		currentSpeed_ = 0.0f;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) {
		currentTurnSpeed_ = -TURN_SPEED;
	}
	else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) {
		currentTurnSpeed_ = TURN_SPEED;
	}
	else {
		currentTurnSpeed_ = 0.0f;
	}
}

void Player::translate(float dx, float dy, float dz) {
	position_.x += dx;
	position_.y = dy;
	position_.z += dz;
}

void Player::rotation(float rx, float ry, float rz) {
	rotation_.x += rx;
	rotation_.y += ry;
	rotation_.z += rz;
}


