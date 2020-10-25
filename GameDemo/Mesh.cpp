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
	loadAnimation(scene, animation);
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

	//loop through each bone
	for (unsigned int i = 0; i < nBoneCount; i++)
	{
		aiBone* bone = mesh->mBones[i];
		glm::mat4 m = assimpToGlmMatrix(bone->mOffsetMatrix);
		boneInfo[bone->mName.C_Str()] = { i, m };

		//loop throuth each vertex have this bone
		for (int j = 0; j < bone->mNumWeights; j++)
		{
			//weight ID
			unsigned int id = bone->mWeights[j].mVertexId;
			//weight
			float weight = bone->mWeights[j].mWeight;
			boneCount[id] ++;
			switch (boneCount[id])
			{
			case 1:
				verticesOutput[id].boneIds.x = i;
				verticesOutput[id].boneWeights.x = weight;
				break;

			case 2:
				verticesOutput[id].boneIds.y = i;
				verticesOutput[id].boneWeights.y = weight;
				break;

			case 3:
				verticesOutput[id].boneIds.z = i;
				verticesOutput[id].boneWeights.z = weight;
				break;

			case 4:
				verticesOutput[id].boneIds.w = i;
				verticesOutput[id].boneWeights.w = weight;
				break;

			default:
				break;
			}
		}
	}

	//normalize weights to make all weights sum
	for (int i = 0; i < verticesOutput.size(); i++)
	{
		glm::vec4 & boneWeights = verticesOutput[i].boneWeights;
		float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;

		if (totalWeight > 0.0f)
		{
			verticesOutput[i].boneWeights = glm::vec4
			(
				boneWeights.x / totalWeight,
				boneWeights.y / totalWeight,
				boneWeights.z / totalWeight,
				boneWeights.w / totalWeight
			);
		}
	}

	//load indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indicesOutput.push_back(face.mIndices[j]);
		}
	}
	
	readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
}

//a recursive function to read all bone and form skeleton
bool Mesh::readSkeleton(Bone & boneOutput, aiNode * node, std::unordered_map<std::string, std::pair<int, glm::mat4>>& boneInfoTable)
{
	//if node is actually a bone
	if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
	{
		boneOutput.name = node->mName.C_Str();
		boneOutput.id = boneInfoTable[boneOutput.name].first;
		boneOutput.offset = boneInfoTable[boneOutput.name].second;

		for (int i = 0; i < node->mNumChildren; i++)
		{
			Bone child;
			readSkeleton(child, node->mChildren[i], boneInfoTable);
			boneOutput.children.push_back(child);
		}
		return true;
	}
	//find bones in children
	else
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable))
			{
				return true;
			}
		}
	}

	return false;
}

void Mesh::loadAnimation(const aiScene * scene, Animation & animation)
{
	//loading first Animation
	aiAnimation* anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0f)
	{
		animation.ticksPerSecond = anim->mTicksPerSecond;
	}
	else
	{
		animation.ticksPerSecond = 1;
	}

	animation.duration = anim->mDuration * anim->mTicksPerSecond;
	animation.boneTransforms;

	//load position rotation and scale for each bone
	//each channel represents each bone
	for (int i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* channel = anim->mChannels[i];
		BoneTransformTrack track;
		for (int j = 0; j < channel->mNumPositionKeys; j++)
		{
			track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
			track.positions.push_back(assimpToClmVec3(channel->mPositionKeys[j].mValue));
		}
		for (int j = 0; j < channel->mNumRotationKeys; j++)
		{
			track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
			track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));
		}
		for (int j = 0; j < channel->mNumScalingKeys; j++)
		{
			track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
			track.scales.push_back(assimpToClmVec3(channel->mScalingKeys[j].mValue));
		}
		animation.boneTransforms[channel->mNodeName.C_Str()] = track;
	}
}

