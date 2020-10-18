#include <iostream>

#include "Mesh.h"
#include "struct.h"

Mesh::Mesh()
{}

//RawModel Mesh::AmLoder(std::string fileName)
//{
//	Assimp::Importer importer;
//
//	const char* filePath = 0;
//
//	const aiScene* scene = importer.ReadFile(filePath,
//		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//	{
//		std::cout << "ERROR: [Mesh::AmLoader] :" << importer.GetErrorString() << "!" << std::endl;
//	}
//	aiMesh* mesh = scene->mMeshes[0];
//
//	glm::mat4 globaInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
//	globaInverseTransform = glm::inverse(globaInverseTransform);
//
//	
//
//	return RawModel ;
//}

