#pragma once

#include <gtx\transform.hpp>

#include "DisplayManager.h"

class Player {

public:
	Player(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	void move();
	void checkInputs();
	void translate(float dx, float dy, float dz);
	void rotation(float rx, float ry, float rz);

	glm::vec3 getPosition() { return position_; }
	glm::vec3 getRotation() { return rotation_; }
	glm::vec3 getScale() { return scale_; }

private:

	//角色位置
	glm::vec3 position_;
	//角色旋转
	glm::vec3 rotation_;
	//大小缩放
	glm::vec3 scale_;

	//现在速度
	float currentSpeed_;

	//现在转向速度
	float currentTurnSpeed_;
};