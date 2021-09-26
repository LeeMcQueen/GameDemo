#include <vector>
#include <glm.hpp>

#include "RawModel.h"
#include "std_image.h"

#pragma once

class RawModel;

class Loader {

public:
	//创建VAO
	GLuint createVAO();

	//VAO单参数加载
	RawModel loadToVAO(std::vector<float> &positions, int dimensions);

	//VAO多参数加载
	RawModel loadToVao(std::vector<glm::vec3> vertices
		, std::vector<glm::vec2> textureCoords
		, std::vector<glm::vec3> normals
		, std::vector<int> indices);

	//纹理图像加载
	GLuint loadTexture(const char *filename);
	GLuint loadCubeMap(const std::vector < std::string > &fileName);

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





