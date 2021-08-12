#include "Player.h"

const float RUN_SPEED = 25.0f;
const float TURN_SPEED = 100.0f;

void Player::move() {
	checkInputs();


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
	position_.x += dx;
	position_.y += dy;
	position_.z += dz;
}
