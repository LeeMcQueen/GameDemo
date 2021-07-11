#pragma once

#include <unordered_map>

#include "BoneTransformTrack.h"

class Animation{

public:

	Animation() {}
	~Animation() {}

	void setDuration(const float duration) { duration_ = duration; }
	void setTicksPerSecond(const float ticksPerSecond) { ticksPerSecond_ = ticksPerSecond; }
	void setBoneTransforms(const std::unordered_map<std::string, BoneTransformTrack> boneTransforms) { boneTransforms_ = boneTransforms; }
	float getDuration() const { return duration_; }
	float getTicksPerSecond() const { return ticksPerSecond_; }
	std::unordered_map<std::string, BoneTransformTrack> getTicksperSecond() const { return boneTransforms_; }

	float duration_;
	float ticksPerSecond_ = 1.0f;
	std::unordered_map<std::string, BoneTransformTrack> boneTransforms_;
};