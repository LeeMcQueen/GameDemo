﻿#pragma once

#include "RawModel.h"
#include "Vertex.h"
#include "Bone.h"
#include "struct.h"
#include "Animation.h"

#include <vector>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <unordered_map>

class AnimaModelLoader {

public:

	AnimaModelLoader() {};
	~AnimaModelLoader() {};
	//使用Assimp通过文件名得到Scene
	void loadAssimpScene(const char *filePath);
	//通过Scene得到模型数据
	void loadAssimpModel(const aiScene *scene,
		aiMesh *mesh,
		std::vector<Vertex> &verticesOutput,
		std::vector<unsigned int> &indicesOutput,
		Bone &skeletonOutput,
		unsigned int &nBoneCount);
	//遍历骨骼结构
	bool readSkeleton(Bone &boneOutput, aiNode *node, std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfoTable);
	void loadAnimation(const aiScene *scene, Animation &animation);

	//输出容器getset
	void setGlobaInverseTransform(const glm::mat4 globalInverseTransform) { globalInverseTransform_ = globalInverseTransform; }
	void setScene(const aiScene *scene) { scene_ = scene; }
	void setVertices(const std::vector<Vertex> vertices) { vertices_ = vertices; }
	void setIndices(const std::vector<unsigned int> indices) { indices_ = indices; }
	void setSkeleton(const Bone skeleton) { skeleton_ = skeleton; }
	void setBoneCount(const unsigned int boneCount) { boneCount_ = boneCount; }
	std::vector<Vertex> getVertices() { return vertices_; }
	std::vector<unsigned int> getIndices() { return indices_; }
	const aiScene* getScene() { return scene_; }
	Bone getSkeleton() { return skeleton_; }
	unsigned int getbBoneCount() { return boneCount_; }
	//全局反向变换
	glm::mat4 getGlobalInverseTransform() const { return globalInverseTransform_; }

private:

	//顶点数据输出容器
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
	const aiScene *scene_;
	Bone skeleton_;
	unsigned int boneCount_;

	glm::mat4 globalInverseTransform_;

};