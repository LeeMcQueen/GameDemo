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
#include "Vertex.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//------------------------------skeleton start------------------------
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <unordered_map>
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"

const char* vertexShaderSource = R"(
	#version 440 core
	layout (location = 0) in vec3 position; 
	layout (location = 1) in vec3 normal;
	layout (location = 2) in vec2 uv;
	layout (location = 3) in vec4 boneIds;
	layout (location = 4) in vec4 boneWeights;

	out vec2 tex_cord;
	out vec3 v_normal;
	out vec3 v_pos;
	out vec4 bw;

	uniform mat4 bone_transforms[50];	//jointTransForms[MAX_JOINTS]
	uniform mat4 view_projection_matrix;
	uniform mat4 model_matrix;

	void main()
	{
		bw = vec4(0);
		if(int(boneIds.x) == 1)
		bw.z = boneIds.x;
		//boneWeights = normalize(boneWeights);
		mat4 boneTransform  =  mat4(0.0);
		boneTransform  +=    bone_transforms[int(boneIds.x)] * boneWeights.x;
		boneTransform  +=    bone_transforms[int(boneIds.y)] * boneWeights.y;
		boneTransform  +=    bone_transforms[int(boneIds.z)] * boneWeights.z;
		boneTransform  +=    bone_transforms[int(boneIds.w)] * boneWeights.w;

		vec4 pos =boneTransform * vec4(position, 1.0);

		gl_Position = view_projection_matrix * model_matrix * pos;

		v_pos = vec3(model_matrix * boneTransform * pos);

		tex_cord = uv;

		v_normal = mat3(transpose(inverse(model_matrix * boneTransform))) * normal;

		v_normal = normalize(v_normal);
	}

	)";
const char* fragmentShaderSource = R"(
	#version 440 core

	in vec2 tex_cord;
	in vec3 v_normal;
	in vec3 v_pos;
	in vec4 bw;
	out vec4 color;

	uniform sampler2D diff_texture;

	vec3 lightPos = vec3(0.2, 1.0, -3.0);
	
	void main()
	{
		vec3 lightDir = normalize(lightPos - v_pos);
		float diff = max(dot(v_normal, lightDir), 0.2);
		vec3 dCol = diff * texture(diff_texture, tex_cord).rgb; 
		color = vec4(dCol, 1);
	}
	)";

//struct Vertex {
//	glm::vec3 position;	//顶点
//	glm::vec3 normal;	//法线
//	glm::vec2 uv;		//图片uv坐标
//	glm::vec4 boneIds;	//骨骼ID(joint关节ID)
//	glm::vec4 boneWeights;	//骨骼权重
//};

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
	//verticesOutput = {};
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
		vertex.setPosition(vector);

		glm::vec3 test = vertex.getPosition();


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

	////加载骨骼数据到顶点数组
	////boneInfo的结构是 谷歌名<string> 矩阵pair
	//std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo = {};
	//std::vector<unsigned int> boneCounts;
	////verticesOutput数据相同长度的boneCounts数据内容都是0
	//boneCounts.resize(verticesOutput.size(), 0);
	////得到模型的总骨骼数
	//nBoneCount = mesh->mNumBones;

	////循环骨骼数量得到？？？
	//for (unsigned int i = 0; i < nBoneCount; i++) {
	//	//得到每个骨头
	//	aiBone *bone = mesh->mBones[i];
	//	//从Assimp格式转换成glm
	//	glm::mat4 matrix = assimpToGlmMatrix(bone->mOffsetMatrix);
	//	//按照骨骼名称(string形式)往里面注入map, map的形式是(i, matrix)
	//	boneInfo[bone->mName.C_Str()] = { i, matrix };

	//	//循环骨骼里面的顶点(通过总权重数就知道有多少个顶点)
	//	for (unsigned int j = 0; j < bone->mNumWeights; j++) {
	//		//当前骨骼的顶点ID
	//		unsigned int id = bone->mWeights[j].mVertexId;
	//		//当前骨骼的顶点的权重值
	//		float weight = bone->mWeights[j].mWeight;
	//		boneCounts[id]++;

	//		//根据
	//		switch (boneCounts[id])
	//		{
	//		case 1:
	//			verticesOutput[id].boneIds.x = i;
	//			verticesOutput[id].boneWeights.x = weight;
	//			break;
	//		case 2:
	//			verticesOutput[id].boneIds.y = i;
	//			verticesOutput[id].boneWeights.y = weight;
	//			break;
	//		case 3:
	//			verticesOutput[id].boneIds.z = i;
	//			verticesOutput[id].boneWeights.z = weight;
	//			break;
	//		case 4:
	//			verticesOutput[id].boneIds.w = i;
	//			verticesOutput[id].boneWeights.w = weight;
	//			break;

	//		default:
	//			break;
	//		}
	//	}
	//}

	////让所有权重的和为1
	//for (unsigned int i = 0; i < verticesOutput.size(); i++) {
	//	//取得所有顶点数组里面的骨骼权重
	//	glm::vec4 &boneWeight = verticesOutput[i].boneWeights;

	//	float totalWeight = boneWeight.x + boneWeight.y + boneWeight.z + boneWeight.w;
	//	if (totalWeight > 0.0f) {
	//		verticesOutput[i].boneWeights = glm::vec4(
	//			boneWeight.x / totalWeight,
	//			boneWeight.y / totalWeight,
	//			boneWeight.z / totalWeight,
	//			boneWeight.w / totalWeight
	//		);
	//	}
	//}

	//for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
	//	aiFace &face = mesh->mFaces[i];
	//	for (unsigned int j = 0; j < face.mNumIndices; j++) {
	//		indicesOutput.push_back(face.mIndices[j]);
	//	}
	//}
	////
	//readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
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
		for (int j = 0; j < channel->mNumRotationKeys; j++) {
			track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
			track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));
		}
		//报错
		for (int j = 0; j < channel->mNumScalingKeys; j++) {
			track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
			track.scales.push_back(assimpToGlmVec3(channel->mScalingKeys[j].mValue));
		}
		animation.boneTransforms[channel->mNodeName.C_Str()] = track;
		//std::cout << "loadAnimation() animation = " << channel->mNodeName.C_Str() << std::endl;
	}
}

//
unsigned int createVertexArray(std::vector<Vertex> &vertices, std::vector<unsigned int> indices) {
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ebo = 0;

	std::cout << vao << std::endl;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) *vertices.size(), &vertices[0], GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneIds));
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneWeights));

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

//
std::pair<unsigned int, float>getTimeFraction(std::vector<float> &times, float &dt) {
	unsigned int segment = 0;

	while (dt > times[segment])
		segment++;

	float start = times[segment - 1];
	float end = times[segment];
	float frac = (dt - start) / (end - start);

	return{ segment, frac };
}

//得到当前姿势
void getPose(Animation &animation, Bone &skeleton, float dt, std::vector<glm::mat4> &output, glm::mat4 &parentTransform, glm::mat4 &globalInverseTransform) {

	BoneTransformTrack &btt = animation.boneTransforms[skeleton.name];
	//余数
	dt = fmod(dt, animation.duration);
	std::pair<unsigned int, float>fp;

	//计算顶点插值
	fp = getTimeFraction(btt.positionTimestamps, dt);
	glm::vec3 position1 = btt.positions[fp.first - 1];
	glm::vec3 position2 = btt.positions[fp.first];
	glm::vec3 position = glm::mix(position1, position2, fp.second);

	//计算旋转插值
	fp = getTimeFraction(btt.rotationTimestamps, dt);
	glm::quat rotation1 = btt.rotations[fp.first - 1];
	glm::quat rotation2 = btt.rotations[fp.first];
	glm::quat rotation = glm::slerp(rotation1, rotation2, fp.second);

	fp = getTimeFraction(btt.scaleTimestamps, dt);
	glm::vec3 scale1 = btt.scales[fp.first - 1];
	glm::vec3 scale2 = btt.scales[fp.first];
	glm::vec3 scale = glm::mix(scale1, scale2, fp.second);

	glm::mat4 positionMat;
	glm::mat4 scaleMat;

	//计算本地变换
	positionMat = glm::translate(positionMat, position);
	glm::mat4 rotationMat = glm::toMat4(rotation);
	scaleMat = glm::scale(scaleMat, scale);

	glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
	glm::mat4 globaTransform = parentTransform * localTransform;

	output[skeleton.ID] = globalInverseTransform * globaTransform * skeleton.offset;

	//更新子骨骼的数组
	for (Bone &child : skeleton.children) {
		getPose(animation, child, dt, output, globaTransform, globalInverseTransform);
	}
}

//------------------------------skeleton end------------------------

//窗口大小变换监听
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

#pragma region 创建窗口
	//初始化glfw
	glfwInit();
	//大版本3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//小版本3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//使用Opengl核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "GameDemo", NULL, NULL);
	//注册窗口大小监听
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//实例化DisplayManager
	DisplayManager* myDisplayManager = new DisplayManager();
	//使用DisplayManager下的CreatManager
	myDisplayManager->CreatManager(window);
#pragma endregion

	//------------------------------skeleton start------------------------
	int windowWidth = 1280, windowHeight = 720;
	bool isRunning = true;

	//使用assimp加载模型
	Assimp::Importer importer;
	const char* filePath = "res/model.dae";
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	//Assimp加载成功判定
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::Assimp :" << importer.GetErrorString() << std::endl;
	}
	aiMesh* mesh = scene->mMeshes[0];

	//顶点数组
	std::vector<Vertex> vertices = {};
	//顶点顺序变量
	std::vector<unsigned int> indices = {};
	unsigned int boneCount = 0;
	//animation数组实例化
	Animation animation;
	//vao
	unsigned int vao = 0;
	//Bone骨骼数组实例化
	Bone skeleton;
	//图片初始化
	unsigned int diffuseTexture;

	glm::mat4 globalInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
	globalInverseTransform = glm::inverse(globalInverseTransform);

	loadModel(scene, mesh, vertices, indices, skeleton, boneCount);
	loadAnimation(scene, animation);

	vao = createVertexArray(vertices, indices);
	diffuseTexture = createTexture("res/diffuse.png");

	glm::mat4 identity;
	std::vector<glm::mat4> currentPose = {};
	currentPose.resize(boneCount, identity);

	//加载shader
	unsigned int shader = createShader(vertexShaderSource, fragmentShaderSource);
	//shader变量的设定和链接
	unsigned int viewProjectionMatrixLocation = glGetUniformLocation(shader, "view_projection_matrix");
	unsigned int modelMatrixLocation = glGetUniformLocation(shader, "model_matrix");
	unsigned int boneMatricesLocation = glGetUniformLocation(shader, "bone_transforms");
	unsigned int textureLocation = glGetUniformLocation(shader, "diff_texture");

	//投影矩阵(projectionMatrix)
	glm::mat4 projectionMatrix = glm::perspective(70.0f, (float)windowWidth / windowHeight, 0.1f, 1000.0f);
	//观察矩阵(viewMatrix)
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.2f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, -1, 0));
	//投影矩阵 + 观察矩阵 TODO
	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

	//------------------------------skeleton end------------------------

	//实例化加载工具
	Loader loader;

	//实例化相机
	Camera camera;

	//实例化渲染器
	MasterRenderer masterRenderer;

	//实例化加载OBJ
	OBJLoader objloader;

	//加载模型顶点信息（3种方法）
	RawModel model = objloader.loadObjModel("person");

	//使用纹理文件名加载纹理
	ModelTexture texture(loader.loadTexture("playerTexture"));
	texture.setShineDamer(100.0f);
	texture.setReflectivity(1.0f);

	//rawModel()  modelTexture()
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

		//transformation
		entity.increasePosition(glm::vec3(0.0f, 0.0f, 0.00f));
		//rotation
		entity.increaseRotation(glm::vec3(0.0f, 0.0001f, 0.0f));

		masterRenderer.processTerrain(terrain2);
		masterRenderer.processTerrain(terrain);
		masterRenderer.processEntity(entity);
		masterRenderer.render(light, camera);
		masterRenderer.cleanUp();	

		//------------------------------skeleton start------------------------
		camera.move();

		//取得当前程序运行时间
		float elapsedTime = glfwGetTime();

		float dAngle = 0.0002;
		
		modelMatrix = glm::rotate(modelMatrix, dAngle, glm::vec3(0, 0.0001, 0));

		getPose(animation, skeleton, elapsedTime, currentPose, identity, globalInverseTransform);
		glUseProgram(shader);
		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(boneMatricesLocation, boneCount, GL_FALSE, glm::value_ptr(currentPose[0]));
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(textureLocation, 0);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		//------------------------------skeleton end------------------------

		//按下Esc就关闭窗口
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		//交换颜色缓冲区
		glfwSwapBuffers(window);
		//检查是否出发相关事件
		glfwPollEvents();
	}

	//销毁New对象
	delete myDisplayManager;

	//销毁GLFW
	glfwTerminate();

}
