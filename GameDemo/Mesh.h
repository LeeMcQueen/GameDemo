#pragma once

#include <glm.hpp>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>
#include <unordered_map>

#include "RawModel.h"

struct Vertex 
{
	glm::vec3 position;
	glm::vec2 textCoords;
	glm::vec3 normals;
	glm::vec4 boneIds = glm::vec4(0.0f);
	glm::vec4 boneWeights = glm::vec4(0.0f);
};

struct Bone
{
	int id = 0;
	std::string name = "";
	glm::mat4 offset = glm::mat4(1.0f);
	std::vector<Bone> children = {};
};

struct BoneTransformTrack 
{
	std::vector<float> positionTimestamps;
	std::vector<float> rotationTimestamps;
	std::vector<float> scaleTimestamps;

	std::vector<glm::vec3> position;
	//std::vector<glm::quat> rotation;
	std::vector<glm::vec3> scale;
};

struct Animation
{
	float duration = 0.0f;
	float ticksPerSecond = 1.0f;
	std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
};

struct Texture 
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	//Constructor
	Mesh();

	void AmLoder(std::string fileName);

	void loadModel(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& verticesOutput,
		std::vector<unsigned int>& indicesOutput, Bone& skeletonOutput, unsigned int &nBoneCount);

private:
	std::vector<Vertex> vertices;
	std::vector< unsigned int> indices;
	unsigned int boneCount = 0;
	Animation animation;
	int vao = 0;
	Bone skeleton;
	int diffuseTexture;

};

