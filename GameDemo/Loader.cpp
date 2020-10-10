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

GLuint Loader::createVAO() 
{
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	vaos.push_back(std::move(vaoID));
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
	// Create a new buffer
	glGenBuffers(1, &vboID);
	// Store the buffer in the list
	vbos.push_back(vboID);
	// Bind the buffer to use it
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	// Store the data in the buffer
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	// Tell OpenGL how and where to store this VBO in the VAO
	//1.VBO Number 2.VBO Size
	glVertexAttribPointer(attribNumber, attribSize, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void Loader::bindIndicesBuffer(int* indices, int& count)
{
	GLuint vboID;
	// Generate a buffer and bind it for use
	glGenBuffers(1, &vboID);
	// Store the buffer in the list
	vbos.push_back(vboID);
	// Bind the buffer to use it
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
	// Store the indices in the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)* count, indices, GL_STATIC_DRAW);
}

GLuint Loader::loadTexture(const char* fileName)
{
	unsigned error;
	unsigned char* image;
	unsigned int width, height;

	/*int numComponents;*/
	//stbi_uc* image = stbi_load(("res/" + string(filename) + ".png").c_str(), &width, &height, &numComponents, 0);

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Store the buffer in the list for delet
	glBindTexture(GL_TEXTURE_2D, 0);
	textures.push_back(texture[0]);

	free(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return texture[0];
}
