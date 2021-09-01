#include <vector>
#include <glm.hpp>

#include "RawModel.h"

#pragma once

class RawModel;

class Loader{

public:
	//创建VAO
	GLuint createVAO();

	//VAO水面加载
	RawModel loadToVAO(std::vector<glm::vec2> &positions, int dimensions);

	RawModel loadToVao(std::vector<glm::vec3> vertices
						, std::vector<glm::vec2> textureCoords
						, std::vector<glm::vec3> normals
						, std::vector<int> indices);

	//load texture from file
	GLuint loadTexture(const char *filename);

private:
	//顶点数组对象
	std::vector<GLuint> vaos;
	//顶点缓存对象
	std::vector<GLuint> vbos;
	std::vector<GLuint> textures;

	void storeDataInAttributeList(GLuint attribNumber, int attribSize, void *data, int dataSize);
	void storeFloatDataInAttributeList(GLuint attribNumber, int attribSize, void *data, int dataSize);
	void bindIndicesBuffer(int *indices, int &count);

	//解绑VAO
	inline void unbindVAO() { glBindVertexArray(0); };
};





