#pragma once

#include <GLFW\glfw3.h>
#include <gtx\transform.hpp>

class Player {

public:
	Player(const glm::vec3) {};

	void move();
	void checkInputs();
	void translate(float dx, float dy, float dz);

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