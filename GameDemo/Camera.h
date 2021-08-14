#pragma once

#include <gtx\transform.hpp>

#include "Player.h"

class Camera {

public:
	Camera(Player player);

	//increase position&rotation into view(camera)Matrix
	void move();

	//观察矩阵函数
	glm::mat4 getViewMatrix() const;
	//相机位置的get函数
	glm::vec3 getPosition() const;
	//get camera rotation
	glm::vec3 getRotation() const;

private:

	//主角类(主角位置，旋转，缩放)
	Player player_;
	//相机位置
	glm::vec3 position_;
	//相机旋转
	glm::vec3 rotation_;
	//相机和主角的距离
	float distanceFromPlayer_;
	//相机围绕主角的角度
	float angleAroundPlayer_;


	//相机到主角远近移动函数
	void calclateZoom();
	//相机高低调整函数
	void calculatePitch();
	//相机围绕主角函数
	void calculateAngleAoundPlayer();
};

