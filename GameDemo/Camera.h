#pragma once

#include <gtx\transform.hpp>

#include "Player.h"

class Camera {

public:
	Camera(Player player);

	//increase position&rotation into view(camera)Matrix
	void move();

	glm::mat4 getViewMatrix() const;
	//get camera position
	glm::vec3 getPosition() const;
	//get camera rotation
	glm::vec3 getRotation() const;

private:

	//主角类(主角位置，旋转，缩放)
	Player player_;
	//camera position
	glm::vec3 position_;
	//camera rotation
	glm::vec3 rotation_;
	//相机和主角的距离
	float distanceFromPlayer_;
	//相机围绕主角的角度
	float angleAroundPlayer_;

	void calclateZoom();

};

