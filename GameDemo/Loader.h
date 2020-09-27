#include <vector>
#include <GL/glew.h>
#include <glm.hpp>

#include "RawModel.h"

#pragma once

class RawModel;

class Loader
{
public:
	//Create VAO
	GLuint createVAO();

	RawModel loadToVAO(std::vector<float> vertices, std::vector<float> textureCoords, 
		std::vector<float> normals , std::vector<int> indices);
	RawModel loadToVao(std::vector<glm::vec3> vertices, std::vector<glm::vec2> textureCoords, 
		std::vector<int> indices);

	//load texture from file
	GLuint loadTexture(const char* filename);

private:
	std::vector<GLuint> vaos;
	std::vector<GLuint> vbos;
	std::vector<GLuint> textures;

	void storeDataInAttributeList(GLuint attribNumber, int attribSize, void* data, int dataSize);

	void bindIndicesBuffer(int* indices, int& count);

	//unbind VAO
	inline void unbindVAO() { glBindVertexArray(0); };
};





