#pragma once

#include <vector>
#include <gtx\transform.hpp>

class Bone{

public:

	Bone() {};
	~Bone() {};

	void setId(const int id) { Id_ = id; };
	void setName(const std::string name) { name_ = name; };
	void setChildren(std::vector<Bone> children) { children_ = children; };
	void setOffset(glm::mat4 offset) { offset_ = offset; };
	int getId() const { return Id_; };
	std::string getName() const { return name_; };
	std::vector<Bone> getChildren() const { return children_; };
	glm::mat4 getOffset() const { return offset_; };
	
	unsigned int Id_;

	std::string name_;
	//éqçúÈé
	std::vector<Bone> children_;

	glm::mat4 offset_;

private:



};