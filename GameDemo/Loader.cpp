#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Loader.h"
#include "ShaderProgram.h"
#include "lodepng.h"
#define STB_IMAGE_IMPLEMENTATION

using namespace std;

//load information to VAO
//data 1.vertices position 2.texture position 
RawModel Loader::loadToVAO(std::vector<float> &positions, int dimensions) {
	GLuint vaoId = createVAO();
	storeFloatDataInAttributeList(0, dimensions, &positions[0], positions.size() * sizeof(float));
	unbindVAO();
	return RawModel(vaoId, positions.size() / dimensions);
}

//模型加载有法线向量
//data 1.vertices position 2.texture position 3.normals
RawModel Loader::loadToVao(std::vector<glm::vec3> vertices, 
	std::vector<glm::vec2> textureCoords,
	std::vector<glm::vec3> normals, 
	std::vector<int> indices) {

	// 创建一个新的VAO
	GLuint vaoID = createVAO();
	int indicesSize = indices.size();
	bindIndicesBuffer(indices.data(), indicesSize);
	// 绑定数据到AttributeList
	storeDataInAttributeList(0, 3, &vertices[0], vertices.size() * sizeof(glm::vec3));
	storeDataInAttributeList(1, 2, &textureCoords[0], textureCoords.size() * sizeof(glm::vec2));
	storeDataInAttributeList(2, 3, &normals[0], normals.size() * sizeof(glm::vec3));
	unbindVAO();
	return RawModel(vaoID, indicesSize);
}

//生成VAO（顶点数组对象）
GLuint Loader::createVAO() {

	GLuint vaoID;
	//创建新的vertexarray
	glGenVertexArrays(1, &vaoID);
	//把创建好的VAO加入VAOS列表里面
	vaos.push_back(std::move(vaoID));
	//绑定VAO（顶点数组对象）
	glBindVertexArray(vaoID);

	return vaoID;
}

void Loader::storeFloatDataInAttributeList(GLuint attribNumber, int attribSize, void *data, int dataSize) {

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
	//1.顶点数据 2.顶点属性的大小 3.数据类型
	glVertexAttribPointer(attribNumber, attribSize, GL_FLOAT, GL_FALSE, 0, nullptr);
}

//VBO封装
void Loader::storeDataInAttributeList(GLuint attribNumber, int attribSize, void *data, int dataSize) {

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
	//1.顶点数据 2.顶点属性的大小 3.数据类型
	glVertexAttribPointer(attribNumber, attribSize, GL_FLOAT, GL_FALSE, 0, nullptr);
}

//use an EBO for higher efficient rendering (less vertex)
//EBO封装
void Loader::bindIndicesBuffer(int *indices, int &count) {

	GLuint eboID;
	// 创建一个buffer用来装EBO
	glGenBuffers(1, &eboID);
	// 加载到VBO的表里面
	vbos.push_back(eboID);
	// 绑定eboID
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	// 使用数据 indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) *count, indices, GL_STATIC_DRAW);
}

GLuint Loader::loadTexture(const char *fileName) {

	unsigned error;
	unsigned char *image;
	unsigned int width, height;

	//Load image using lodepng
	error = lodepng_decode32_file(&image, &width, &height, ("res/" + string(fileName) + ".png").c_str());
	//纹理读取检测
	if (error) {
		cerr << "ERROR: [TextureLoader::loadTexture] Cannot load texture" << fileName << "!" << endl;
		exit(-1);
	}

	//cerr << "info: [textureloader::loadtexture] x:" << width << "!" << endl;
	//cerr << "info: [textureloader::loadtexture] y:" << height << "!" << endl;
	//cerr << "info: [textureloader::loadtexture] texture:" << fileName << "!" << endl;

	//纹理贴图标识
	GLuint texture[1];
	//生成纹理 指定纹理数量是1和纹理标识
	glGenTextures(1, texture);
	//绑定纹理对象 指定2D纹理和纹理标识
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	//设置纹理贴图收缩和扩展的过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//opengl的minimap，远处的物体的纹理质量低
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.1f);
	//设置S，T坐标上的纹理表现形式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//指定的参数，数据生成一个纹理图片
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//生成Mip层
	glGenerateMipmap(GL_TEXTURE_2D);

	//绑定的纹理对象进行清零
	glBindTexture(GL_TEXTURE_2D, 0);
	textures.push_back(texture[0]);

	free(image);

	return texture[0];
}

GLuint Loader::loadCubeMap(const std::vector<std::string> &fileName){

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, numberChannels;

	for (int i = 0; i < fileName.size(); i++) {
		
		auto path = fileName[i];
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &numberChannels, 0);
		if (data) {
			//指定的参数，数据生成一个纹理图片
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}else {
			std::cout << " CubeMap filed to load: " << path << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
