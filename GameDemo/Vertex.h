#pragma once

#include <gtx\transform.hpp>

class Vertex
{
public:

	//构造函数
	Vertex();

	//析构函数
	~Vertex();

	//get和set
	void setPosition(const glm::vec3 position);
	void setNormal(const glm::vec3 normal);
	glm::vec3 getPosition() const { return position_; };

private:

	//顶点坐标
	glm::vec3 position_;

	//法线坐标
	glm::vec3 normal_;

	//纹理坐标
	glm::vec2 texture_;

	//骨骼ID
	glm::vec4 boneIds_;

	//骨骼权重
	glm::vec4 boneWeights_;
};