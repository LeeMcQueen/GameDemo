#pragma once

#include <glm.hpp>
#include <gtx/string_cast.hpp>

#include "RawModel.h"
#include "Loader.h"

using namespace std;

class OBJLoader{

public:

	//OBJ模型加载方法
	static RawModel loadObjModel(const std::string &fileName);

private:
	//best OBJLoader
	static void ProcessVertices(char *vertexData, std::vector<int> &indices, std::vector<glm::vec2> &tempTextures,
		std::vector<glm::vec2> &textures, std::vector<glm::vec3> &tempNormals, std::vector<glm::vec3> &normals);

	//OBJ模型加载 顶点位置信息
	static vector<float> verticesArray;
	//OBJ模型加载 纹理位置信息
	static vector<float> texturesArray;
	//OBJ模型加载 法线位置信息
	static vector<float> normalsArray;
	//OBJ模型加载 数据顺序信息
	static vector<int> indices;
};

