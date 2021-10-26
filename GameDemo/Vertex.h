#pragma once

#include <gtx\transform.hpp>

class Vertex {

public:

	//构造函数
	Vertex() {};

	//析构函数
	~Vertex() {};

	//Vertex类的get和set
	void setPosition(const glm::vec3 position) { position_ = position; }
	void setNormal(const glm::vec3 normal) { normal_ = normal; }
	void setTexture(const glm::vec2 texture) { texture_ = texture; }
	void setBoneIds(const glm::vec4 boneIds) { boneIds_ = boneIds; }
	void setBoneIds_x(unsigned int boneIds_x) { boneIds_.x = boneIds_x; }
	void setBoneIds_y(unsigned int boneIds_y) { boneIds_.y = boneIds_y; }
	void setBoneIds_z(unsigned int boneIds_z) { boneIds_.z = boneIds_z; }
	void setBoneIds_w(unsigned int boneIds_w) { boneIds_.w = boneIds_w; }
	void setBoneWeights(const glm::vec4 boneWeights) { boneWeights_ = boneWeights; }
	void setBoneWeights_x(float boneWeights_x) { boneWeights_.x = boneWeights_x; }
	void setBoneWeights_y(float boneWeights_y) { boneWeights_.y = boneWeights_y; }
	void setBoneWeights_z(float boneWeights_z) { boneWeights_.z = boneWeights_z; }
	void setBoneWeights_w(float boneWeights_w) { boneWeights_.w = boneWeights_w; }
	void setTangent(const glm::vec3 tangent) { tangent_ = tangent; }
	void setBitangent(const glm::vec3 biTangent) { bitangent_ = biTangent; }
	glm::vec3 getPosition() const { return position_; }
	glm::vec3 getNormal() const { return normal_; }
	glm::vec2 getTexture() const { return texture_; }
	glm::vec4 getBoneIds() const { return boneIds_; }
	glm::vec4 getBoneWieghts() const { return boneWeights_; }
	glm::vec3 getTangent() const { return tangent_; }
	glm::vec3 getBitangent() const { return bitangent_; }

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

	//法线贴图成员变量 切线
	glm::vec3 tangent_;
	//法线贴图成员变量 副切线
	glm::vec3 bitangent_;
};