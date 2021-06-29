#pragma once

#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "RawModel.h"
#include "Loader.h"

using namespace std;

class OBJLoader
{
public:
	//Assimp模型加载
	struct Bone {
		int ID = 0;
		std::string name = "";
		std::vector<Bone> children = {};

		glm::mat4 offset;	//反矩阵
	};
	//Assimp模型加载
	struct Vertex {
		glm::vec3 position;	//顶点
		glm::vec3 normal;	//法线
		glm::vec2 uv;		//图片uv坐标
		glm::vec4 boneIds;	//骨骼ID(joint关节ID)
		glm::vec4 boneWeights;	//骨骼权重
	};

	//OBJ模型加载方法
	static RawModel loadObjModel(const std::string& fileName);
	//Assimp加载模型
	static RawModel loadModel(const aiScene *scene, aiMesh *mesh, std::vector<Vertex> &verticesOutput, std::vector<unsigned int> &indicesOutput, Bone &skeletonOutput, unsigned int &nBoneCount);

private:
	//best OBJLoader
	static void ProcessVertices(char *vertexData, std::vector<int> &indices, std::vector<glm::vec2> &tempTextures,
		std::vector<glm::vec2>& textures, std::vector<glm::vec3> &tempNormals, std::vector<glm::vec3> &normals);

	//OBJ模型加载 顶点位置信息
	static vector<float> verticesArray;
	//OBJ模型加载 纹理位置信息
	static vector<float> texturesArray;
	//OBJ模型加载 法线位置信息
	static vector<float> normalsArray;
	//OBJ模型加载 数据顺序信息
	static vector<int> indices;

};

