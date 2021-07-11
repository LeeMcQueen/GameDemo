#pragma once

#include "RawModel.h"
#include "Vertex.h"
#include "Bone.h"
#include "struct.h"

#include <vector>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <unordered_map>

class AnimaLoader {

public:

	//使用Assimp通过文件名得到Scene
	void loadAssimpScene(const char *filePath);
	//通过Scene得到模型数据
	void loadAssimpModel(const aiScene* scene,
		aiMesh* mesh, std::vector<Vertex>& verticesOutput,
		std::vector<unsigned int>& indicesOutput,
		Bone& skeletonOutput,
		unsigned int& nBoneCount);
	//遍历骨骼结构
	bool readSkeleton(Bone &boneOutput, aiNode *node, std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfoTable);
	//
	glm::mat4 getGlobalInverseTransform();
};