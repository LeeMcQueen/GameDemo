#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>

#include "DisplayManager.h"
#include "RawModel.h"
#include "Loader.h"
#include "ModelTexture.h"
#include "TexturedModel.h"
#include "camera.h"
#include "OBJLoader.h"
#include "Light.h"
#include "MasterRenderer.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//------------------------------skeleton start------------------------
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"

struct Vertex {
	glm::vec3 position;	//顶点
	glm::vec3 normal;	//法线
	glm::vec2 uv;		//图片uv坐标
	glm::vec4 boneIds;	//骨骼ID(joint关节ID)
	glm::vec4 boneWeights;	//骨骼权重
};

struct Bone {
	int ID = 0;
	std::string name = "";
	std::vector<Bone> children = {};

	glm::mat4 offset;	//反矩阵
};

struct BoneTransformTrack {
	std::vector<float> positionTimestamps = {};
	std::vector<float> rotationTimestamps = {};
	std::vector<float> scaleTimestamps = {};

	std::vector<glm::vec3> positions = {};
	std::vector<glm::quat> rotations = {};
	std::vector<glm::vec3> scales = {};
};

struct Animation {
	float duration = 0.0f;
	float ticksPerSecond = 1.0f;
	std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
};

bool readSkeleton(Bone &boneOutput, aiNode *node, std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfoTable) {

	if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end()) {
		boneOutput.name = node->mName.C_Str();
		boneOutput.ID = boneInfoTable[boneOutput.name].first;
		boneOutput.offset = boneInfoTable[boneOutput.name].second;

		for (int i = 0; i < node->mNumChildren; i++) {
			Bone child;
			readSkeleton(child, node->mChildren[i], boneInfoTable);
			boneOutput.children.push_back(child);
		}
		return true;
	}
	else {
		for (int i = 0; i < node->mNumChildren; i++) {
			if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable)) {
				return true;
			}
		}
	}
	return false;
}

void loadModel(const aiScene *scene, aiMesh *mesh, std::vector<Vertex> &verticesOutput, std::vector<unsigned int> &indicesOutput, Bone &skeletonOutput, unsigned int &nBoneCount) {
	//list初始化
	verticesOutput = {};
	indicesOutput = {};
	//取得模型坐标用变量
	Vertex vertex;
	glm::vec3 vector;

	//加载顶点，法线，uv坐标
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		//取得顶点
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		//注意这里就只有一个纹理，需要的话需要动态设定
		vector.x = mesh->mTextureCoords[0][i].x;
		vector.y = mesh->mTextureCoords[0][i].y;
		vertex.uv = vector;

		vertex.boneIds = glm::ivec4(0);
		vertex.boneWeights = glm::vec4(0.0f);

		verticesOutput.push_back(vertex);
	}

	//加载骨骼数据到顶点数组
	//boneInfo的结构是 谷歌名<string> 矩阵pair
	std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo = {};
	std::vector<unsigned int> boneCounts;
	//verticesOutput数据相同长度的boneCounts数据内容都是0
	boneCounts.resize(verticesOutput.size(), 0);
	//得到模型的总骨骼数
	nBoneCount = mesh->mNumBones;

	//循环骨骼数量得到？？？
	for (unsigned int i = 0; i < nBoneCount; i++) {
		//得到每个骨头
		aiBone *bone = mesh->mBones[i];
		//从Assimp格式转换成glm
		glm::mat4 matrix = assimpToGlmMatrix(bone->mOffsetMatrix);
		//按照骨骼名称(string形式)往里面注入map, map的形式是(i, matrix)
		boneInfo[bone->mName.C_Str()] = { i, matrix };

		//循环骨骼里面的顶点(通过总权重数就知道有多少个顶点)
		for (unsigned int j = 0; j < bone->mNumWeights; j++) {
			//当前骨骼的顶点ID
			unsigned int id = bone->mWeights[j].mVertexId;
			//当前骨骼的顶点的权重值
			float weight = bone->mWeights[j].mWeight;
			boneCounts[id]++;

			//根据
			switch (boneCounts[id])
			{
			case 1:
				verticesOutput[id].boneIds.x = i;
				verticesOutput[id].boneWeights.x = weight;
				break;
			case 2:
				verticesOutput[id].boneIds.y = i;
				verticesOutput[id].boneWeights.y = weight;
				break;
			case 3:
				verticesOutput[id].boneIds.z = i;
				verticesOutput[id].boneWeights.z = weight;
				break;
			case 4:
				verticesOutput[id].boneIds.w = i;
				verticesOutput[id].boneWeights.w = weight;
				break;

			default:
				break;
			}

		}
	}

	//让所有权重的和为1
	for (unsigned int i = 0; i < verticesOutput.size(); i++) {
		//取得所有顶点数组里面的骨骼权重
		glm::vec4 &boneWeight = verticesOutput[i].boneWeights;

		float totalWeight = boneWeight.x + boneWeight.y + boneWeight.z + boneWeight.w;
		if (totalWeight > 0.0f) {
			verticesOutput[i].boneWeights = glm::vec4(
				boneWeight.x / totalWeight,
				boneWeight.y / totalWeight,
				boneWeight.z / totalWeight,
				boneWeight.w / totalWeight
			);
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace &face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indicesOutput.push_back(face.mIndices[j]);
		}
	}
	//
	readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
}

//加载动画信息
void loadAnimation(const aiScene *scene, Animation &animation) {

	//加载第一个动画,多个动画加载需要修改
	aiAnimation *anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0f) {
		animation.ticksPerSecond = anim->mTicksPerSecond;
	}
	else {
		animation.ticksPerSecond = 1;
	}

	animation.duration = anim->mDuration * anim->mTicksPerSecond;
	animation.boneTransforms = {};

	for (int i = 0; i < anim->mNumChannels; i++) {
		aiNodeAnim *channel = anim->mChannels[i];
		BoneTransformTrack track;

		for (int j = 0; j < channel->mNumPositionKeys; j++) {
			track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
			track.positions.push_back(assimpToGlmVec3(channel->mPositionKeys[j].mValue));
		}
		for (int j = 0; j < channel->mNumRotationKeys; i++) {
			track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
			track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));
		}

		//报错
		for (int j = 0; j < channel->mNumScalingKeys; j++) {
			track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
			track.scales.push_back(assimpToGlmVec3(channel->mScalingKeys[j].mValue));
		}
		animation.boneTransforms[channel->mNodeName.C_Str()] = track;
	}
}

//
unsigned int createVertexArray(std::vector<Vertex> &vertices, std::vector<unsigned int> indices) {
	unsigned int vao, vbo, ebo;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) *vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneIds));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneWeights));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	return vao;
}

unsigned int createTexture(std::string filepath) {
	unsigned int textureId = 0;
	int width, height, nrChannels;
	byte* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 4);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

//------------------------------skeleton end------------------------

//窗口大小变换监听
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

	//------------------------------skeleton start------------------------
	//使用assimp加载模型
	Assimp::Importer importer;
	const char* filePath = "res/model.dae";
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	//Assimp加载成功判定
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::Assimp :" << importer.GetErrorString() << std::endl;
	}
	aiMesh* mesh = scene->mMeshes[0];

	//变量初始化
	std::vector<Vertex> vertices = {};	//顶点数组
	std::vector<unsigned int> indices = {};	//顶点顺序变量
	unsigned int boneCount = 0;
	Animation animation;	//animation数组实例化
	unsigned int vao = 0;
	Bone skeleton;	//Bone骨骼数组实例化
	unsigned int diffuseTexture;	//图片初始化

	glm::mat4 globaInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
	globaInverseTransform = glm::inverse(globaInverseTransform);

	loadModel(scene, mesh, vertices, indices, skeleton, boneCount);
	loadAnimation(scene, animation);

	vao = createVertexArray(vertices, indices);
	diffuseTexture = createTexture("res/playerTexture.png");

	//------------------------------skeleton end------------------------

#pragma region 创建窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "GameDemo", NULL, NULL);
	//注册窗口大小监听
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//open window
	DisplayManager* myDisplayManager = new DisplayManager();
	myDisplayManager->CreatManager(window);
#pragma endregion

	//临时数据
	Loader loader;

	//相机
	Camera camera;

	//渲染器
	MasterRenderer masterRenderer;

	//加载OBJ数据
	OBJLoader objloader;

	//load OBJModel 3 function
	/*RawModel model = myOBJLoader->tinyOBJLoader("stall");*/
	/*RawModel model = myOBJLoader->loadModel("stall");*/
	RawModel model = objloader.loadObjModel("person");

	//使用纹理文件名加载纹理
	ModelTexture texture(loader.loadTexture("playerTexture"));
	texture.setShineDamer(100.0f);
	texture.setReflectivity(1.0f);

	TexturedModel texturedModel(model, texture);

	//加载模型
	Entity entity(texturedModel, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	//加载灯光
	Light light(glm::vec3(400, 400, 200), glm::vec3(1, 1, 1));
	//加载地面
	Terrain terrain(0, 0, loader, ModelTexture(loader.loadTexture("grassy2")));
	Terrain terrain2(100, 10, loader, ModelTexture(loader.loadTexture("grassy3")));

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//------------------------------skeleton start------------------------

		//------------------------------skeleton end------------------------

		glClearColor(0.5f, 0.1f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//transformation
		entity.increasePosition(glm::vec3(0.0f, 0.0f, 0.00f));
		//rotation
		entity.increaseRotation(glm::vec3(0.0f, 0.0001f, 0.0f));

		camera.move();

		masterRenderer.processTerrain(terrain2);
		masterRenderer.processTerrain(terrain);
		masterRenderer.processEntity(entity);
		masterRenderer.render(light, camera);
		masterRenderer.cleanUp();

#pragma region 关闭窗口和销毁窗口
		//按下Esc就关闭窗口
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		//交换颜色缓冲区
		glfwSwapBuffers(window);
		//检查是否出发相关事件
		glfwPollEvents();
	}

	//销毁GLFW
	glfwTerminate();
#pragma endregion

}
