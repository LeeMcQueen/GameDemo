#define GLEW_STATIC
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <string>

#pragma once
class DisplayManager {

public:
	constexpr static int WIDTH = 1280;
	constexpr static int HEIGHT = 720;

	//创建窗口函数
	static int CreatManager(GLFWwindow *window);

	float getCurrentFrameTime() const { return glfwGetTime(); }
	float getLastFrameTime() const { return lastFrameTime_; }
	float getDeltaTime() const { return deltaTime_; }
	void setLastFrameTime(const float lastFrameTime) { lastFrameTime_ = lastFrameTime; }
	void setDeltaTime(const float deltaTime) { deltaTime_ = deltaTime; }

private:

	float lastFrameTime_ = 0.0f;
	float deltaTime_ = 0.0f;
	float currentFrameTime_ = 0.0f;
};

