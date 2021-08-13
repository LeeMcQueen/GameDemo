#include <iostream>
#include "Player.h"

const float RUN_SPEED = 0.5f;
const float TURN_SPEED = 5.0f;

void Player::move() {

	//键盘控制
	checkInputs();

	DisplayManager displayManager;
	float deltaTime = displayManager.getCurrentFrameTime();

	rotation_ = currentTurnSpeed_;

	/*
	sin(θ) = x/distance 
	cos(θ) = z/distance 
	*/
	float distance = currentSpeed_ * RUN_SPEED;
	
	float deltaX = distance * std::sin(glm::radians(currentTurnSpeed_));
	float deltaY = distance * std::cos(glm::radians(currentTurnSpeed_));

	translate(deltaX, deltaY, 0.0f);
}

void Player::checkInputs(){
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_I) == GLFW_PRESS) {
		currentSpeed_ = -RUN_SPEED;
	}else if(glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_K) == GLFW_PRESS){
		currentSpeed_ = RUN_SPEED;
	}else {
		currentSpeed_ = 0.0f;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_L) == GLFW_PRESS) {
		currentTurnSpeed_ = -TURN_SPEED;
	}
	else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_J) == GLFW_PRESS) {
		currentTurnSpeed_ = TURN_SPEED;
	}else {
		currentTurnSpeed_ = 0.0f;
	}
}

void Player::translate(float dx, float dy, float dz){
	position_.x = dx;
	position_.y = dy;
	position_.z = dz;
}


