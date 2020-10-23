#include <iostream>

#include "Mesh.h"
#include "struct.h"

Mesh::Mesh()
{}

void Mesh::AmLoader(std::string fileName)
{
	Assimp::Importer importer;

	const char* filePath = 0;

	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: [Mesh::AmLoader] :" << importer.GetErrorString() << "!" << std::endl;
	}
	aiMesh* mesh = scene->mMeshes[0];

	glm::mat4 globaInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
	globaInverseTransform = glm::inverse(globaInverseTransform);

	loadModel(scene, mesh, vertices, indices, skeleton, boneCount);
}

//1. 2.all mesh infomation 3.model struct 4. 5. 6.
void Mesh::loadModel(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& verticesOutput, 
	std::vector<unsigned int>& indicesOutput, Bone& skeletonOutput, unsigned int &nBoneCount)
{
	//output list
	verticesOutput = {};
	indicesOutput = {};

	//load position, normal, textureCoords
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//process position
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.positions = vector;

		//process normal
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normals = vector;

		//process textureCoords
		glm::vec2 vec;
		vec.x = mesh->mTextureCoords[0][i].x;
		vec.y = mesh->mTextureCoords[0][i].y;
		vertex.textCoords = vec;

		vertex.boneIds = glm::ivec4(0);
		vertex.boneWeights = glm::vec4(0.0f);

		//set position, normals, textureCoords to list
		verticesOutput.push_back(vertex);
	}

	//-------------------bone------------------------------------------
	//load boneData to vertices
	std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo;
	std::vector<unsigned int>boneCount;

	boneCount.resize(verticesOutput.size(), 0);
	nBoneCount = mesh->mNumBones;

	for (unsigned int i = 0; i < nBoneCount; i++)
	{
		aiBone* bone = mesh->mBones[i];

	}
}

