#include "Player.h"

const float RUN_SPEED = 0.5f;
const float TURN_SPEED = 0.5f;

void Player::move() {
	checkInputs();

	DisplayManager displayManager;
	float deltaTime = displayManager.getCurrentFrameTime();

	rotate(0.0f, currentTurnSpeed_, 0.0f);

	/*
	sin(É∆) = x/distance 
	cos(É∆) = z/distance 
	*/
	float distance = currentSpeed_ * RUN_SPEED;
	float deltaX = distance * std::sin(glm::radians(getRotation().y));
	float deltaY = distance * std::cos(glm::radians(getRotation().y));

	translate(deltaX, 0.0f, deltaY);
}

void Player::checkInputs(){
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_I) == GLFW_PRESS) {
		currentSpeed_ = RUN_SPEED;
	}else if(glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_K) == GLFW_PRESS){
		currentSpeed_ = -RUN_SPEED;
	}else {
		currentSpeed_ = 0.0f;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_L) == GLFW_PRESS) {
		currentTurnSpeed_ = -currentTurnSpeed_;
	}
	else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_J) == GLFW_PRESS) {
		currentTurnSpeed_ = currentTurnSpeed_;
	}else {
		currentTurnSpeed_ = 0.0f;
	}
}

void Player::translate(float dx, float dy, float dz){
	position_.x = dx;
	position_.y = dy;
	position_.z = dz;
}

void Player::rotate(float dx, float dy, float dz){
	rotation_.x += dx;
	rotation_.y += dy;
	rotation_.z += dz;
}
