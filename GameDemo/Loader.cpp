#include "Loader.h"
#include "ShaderProgram.h"
#include "lodepng.h"
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

//Info : you can check modelLoader.cpp

//生成VAO（顶点数组对象）
GLuint Loader::createVAO() 
{
	GLuint vaoID;

	glGenVertexArrays(1, &vaoID);
	vaos.push_back(std::move(vaoID));
	//绑定VAO（顶点数组对象）
	glBindVertexArray(vaoID);

	return vaoID;
}

//load information to VAO
//data 1.vertices position 2.texture position 3.normals
RawModel Loader::loadToVAO(std::vector<float> vertices, std::vector<float> textureCoords,
	std::vector<float> normals, std::vector<int> indices)
{
	// create a new VAO
	GLuint vaoID = createVAO();
	int indicesSize = indices.size();
	bindIndicesBuffer(indices.data(), indicesSize);
	// Store the data in attribute lists
	storeDataInAttributeList(0, 3, &vertices[0], vertices.size()* sizeof(float));
	storeDataInAttributeList(1, 2, &textureCoords[0], textureCoords.size() * sizeof(float));
	storeDataInAttributeList(2, 3, &normals[0], normals.size()* sizeof(float));
	unbindVAO();
	return RawModel(vaoID, indicesSize);
}

//load information to VAO
//data 1.vertices position 2.texture position 
RawModel Loader::loadToVao(std::vector<glm::vec3> vertices, std::vector<glm::vec2> textureCoords, 
	std::vector<glm::vec3> normals, std::vector<int> indices)
{
	// create a new VAO
	GLuint vaoID = createVAO();
	int indicesSize = indices.size();
	bindIndicesBuffer(indices.data(), indicesSize);
	// Store the data in attribute lists
	storeDataInAttributeList(0, 3, &vertices[0], vertices.size() * sizeof(glm::vec3));
	storeDataInAttributeList(1, 2, &textureCoords[0], textureCoords.size() * sizeof(glm::vec2));
	storeDataInAttributeList(2, 3, &normals[0], normals.size() * sizeof(glm::vec3));
	unbindVAO();
	return RawModel(vaoID, indicesSize);
}



void Loader::storeDataInAttributeList(GLuint attribNumber, int attribSize, void* data, int dataSize)
{
	GLuint vboID;
	//创建VBO（顶点缓冲对象）
	glGenBuffers(1, &vboID);

	//把VBO储存到表里
	vbos.push_back(vboID);

	//绑定VBO（顶点缓冲对象）
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	//把数据存储进绑定的缓存里面 1.缓存类型 2.数据大小 3.存储的数据（数组） 4.管理类型（数据不会或几乎不会改变）
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);

	//把VBO（顶点缓冲对象）存储进入VAO（顶点数组对象）
	//1.顶点属性 2.顶点属性的大小 3.数据类型
	glVertexAttribPointer(attribNumber, attribSize, GL_FLOAT, GL_FALSE, 0, nullptr);
}

//use an EBO for higher efficient rendering (less vertex)
void Loader::bindIndicesBuffer(int* indices, int& count)
{
	GLuint eboID;
	// Generate a buffer and bind it for use
	glGenBuffers(1, &eboID);

	// Store the buffer in the list
	vbos.push_back(eboID);

	// Bind the buffer to use it
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);

	// Store the indices in the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)* count, indices, GL_STATIC_DRAW);
}

GLuint Loader::loadTexture(const char* fileName)
{
	unsigned error;
	unsigned char* image;
	unsigned int width, height;

	//Load image using lodepng
	error = lodepng_decode32_file(&image, &width, &height, ("res/" + string(fileName) + ".png").c_str());
	//check load image
	if (error)
	{
		cerr << "ERROR: [TextureLoader::loadTexture] Cannot load texture" << fileName << "!" << endl;
		exit(-1);
	}

	cerr << "INFO: [TextureLoader::loadTexture] x:" << width << "!" << endl;
	cerr << "INFO: [TextureLoader::loadTexture] y:" << height << "!" << endl;
	cerr << "INFO: [TextureLoader::loadTexture] texture:" << fileName << "!" << endl;

	//check
	//Generate texture
	GLuint texture[1];
	//Generate and bind a OpenGL texture
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	//basic
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Store the buffer in the list for delet
	glBindTexture(GL_TEXTURE_2D, 0);
	textures.push_back(texture[0]);

	free(image);

	return texture[0];
}
