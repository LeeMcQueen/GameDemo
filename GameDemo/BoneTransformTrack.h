#pragma once

#include <vector>
#include <gtx\transform.hpp>

class BoneTransformTrack {

public:

	BoneTransformTrack() {};
	~BoneTransformTrack() {};

	void setPositionTimestamps(const std::vector<float> positionTimestamps) { positionTimestamps_ = positionTimestamps; }
	void setRotationTimestamps(const std::vector<float> rotationTimestamps) { rotationTimestamps_ = rotationTimestamps; }
	void setScaleTimestamps(const std::vector<float> scaleTimestamps) { scaleTimestamps_ = scaleTimestamps; }
	void setPosition(const std::vector<glm::vec3> position) { positions_ = position; }
	void setRotation(const std::vector<glm::quat> rotation) { rotations_ = rotation; }
	void setScale(const std::vector<glm::vec3> scales) { scales_ = scales; }
	std::vector<float> getPositionTimestamps() const { return positionTimestamps_; }
	std::vector<float> getRotationTimestamps() const { return rotationTimestamps_; }
	std::vector<float> getScaleTimetamps() const { return scaleTimestamps_; }
	std::vector<glm::vec3> getPositions() const { return positions_; }
	std::vector<glm::quat> getRotations() const { return rotations_; }
	std::vector<glm::vec3> getScales() const { return scales_; }

	std::vector<float> positionTimestamps_;
	std::vector<float> rotationTimestamps_;
	std::vector<float> scaleTimestamps_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::quat> rotations_;
	std::vector<glm::vec3> scales_;

};