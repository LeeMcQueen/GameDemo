#pragma once

#include <gtx\transform.hpp>

#include "DisplayManager.h"

class Player {

public:
	Player() {};

	void move();
	void checkInputs();
	void translate(float dx, float dy, float dz);

	glm::vec3 getPosition() { return position_; }
	float getRotation() { return rotation_; }
	glm::vec3 getScale() { return scale_; }

private:

	//角色位置
	glm::vec3 position_;

	//角色旋转
	float rotation_;

	//大小缩放
	glm::vec3 scale_;

	//现在速度
	float currentSpeed_;

	//现在转向速度
	float currentTurnSpeed_;
};