#ifndef CAMERA_H
#define CAMERA_H

#include <gtx\transform.hpp>
#include <GLFW/glfw3.h>

class Camera {

public:
	Camera();

	//increase position&rotation into view(camera)Matrix
	void move(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	//观察矩阵函数
	glm::mat4 getViewMatrix() const;
	//相机位置的get函数
	glm::vec3 getPosition() const;
	//相机旋转的get函数
	glm::vec3 getRotation() const;
	//设定相机position
	void setPosition(glm::vec3 position) { position_ = position; }
	void setviewDirection(glm::vec3 viewDirection) { viewDirection_ = viewDirection; };

private:
	//相机位置
	glm::vec3 position_;
	//相机旋转
	glm::vec3 rotation_;
	//相机和主角的距离
	float distanceFromPlayer_;
	//相机围绕主角的角度
	float angleAroundPlayer_;

	glm::vec3 viewDirection_;

	//相机到主角远近移动函数
	void calclateZoom();
	//相机高低调整函数
	void calculatePitch();
	//相机围绕主角函数
	void calculateAngleAoundPlayer();
	//主角相机水平距离
	float calculateHorizontaDistance();
	//主角相机高度距离
	float calculateVerticalDistance();
	//相机位置
	void calculateCameraPosition(float horizDistance, float verticDistance, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

};

#endif

