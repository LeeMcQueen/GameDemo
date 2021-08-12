#pragma once

#include <gtx\transform.hpp>

#include "struct.h"
#include "TexturedModel.h"
#include "RawModel.h"

class Entity {

public:

	//构造函数
	Entity(const TexturedModel &model,
		const glm::vec3 position,
		const glm::vec3 rotation,
		const glm::vec3 scale
	);

	//析构函数
	~Entity();

	//位置移动
	void increasePosition(const glm::vec3 position);
	//旋转移动
	void increaseRotation(const glm::vec3 rotation);

	//得到OBJ模型和纹理
	TexturedModel GetModel() const;
	//get和set
	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::vec3 getScale() const;
	void setPosition(const glm::vec3 position);
	void setRotation(const glm::vec3 rotation);
	void setScale(const glm::vec3 scale);

private:

	//Entity position
	glm::vec3 position_;

	//Entity rotation
	glm::vec3 rotation_;

	//scale value
	glm::vec3 scale_;

	//Entity texture
	TexturedModel model_;
};

