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
	RawModel loadToVAO(std::vector<float> &position, int dimensions);
	RawModel FUCKvao(std::vector<float> &position, int dimensions);

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
	void bindIndicesBuffer(int *indices, int &count);

	static unsigned int createVBO();
	static std::vector<unsigned int> _vbos;
	template <typename T>
	static void storeData(unsigned int attNum, unsigned int count, const std::vector<T>& data) {
		//create a vbo
		unsigned int vboID = createVBO();
		//store it for later
		_vbos.push_back(vboID);
		//bind it so it can be used
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		//store data in the vbo: type of buffer, size, actual data, draw type
		//NOTE: &data.front() returns a pointer to the beginning of the vector, so it's like passing an array which is exactly what openGL is expecting
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), &data.front(), GL_STATIC_DRAW);
		//put vbo into the vao: num of attribute list, length of vertex, type of data, distance between vertices, offset from start
		glVertexAttribPointer(attNum, count, GL_FLOAT, GL_FALSE, 0, nullptr);
		//unbind the vbo, 0 unbinds the current one
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//解绑VAO
	inline void unbindVAO() { glBindVertexArray(0); };
};





