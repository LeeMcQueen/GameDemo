
//#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>


#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//------------------------------animation start------------------------
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
//------------------------------animation start------------------------
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"

#include "DisplayManager.h"
#include "RawModel.h"
#include "Loader.h"
#include "ModelTexture.h"
#include "TexturedModel.h"
#include "camera.h"
#include "OBJLoader.h"
#include "Light.h"
#include "MasterRenderer.h"
#include "AnimaModelLoader.h"

#include "Vertex.h"
#include "Bone.h"
#include "BoneTransformTrack.h"
#include "Animation.h"
#include "Cloth.h"  //Points
#include "Rigid.h"
#include "Program.h"
#include "Display.h"

#define AIR_FRICTION 0.02
#define TIME_STEP 0.01

const char* vertexShaderSource = R"(
	#version 440 core
	layout (location = 0) in vec3 position; 
	layout (location = 1) in vec3 normal;
	layout (location = 2) in vec2 texture;
	layout (location = 3) in vec4 boneIds;
	layout (location = 4) in vec4 boneWeights;

	out vec2 tex_cord;
	out vec3 v_normal;
	out vec3 v_pos;
	out vec4 bw;

	uniform mat4 bone_transforms[50];		//jointTransForms[MAX_JOINTS]测试
	//uniform mat4 view_projection_matrix;
	uniform mat4 view_Matrix;
	uniform mat4 projection_Matrix;
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

		gl_Position = projection_Matrix * view_Matrix * model_matrix * pos;

		v_pos = vec3(model_matrix * boneTransform * pos);

		tex_cord = texture;

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

	vec3 lightPos = vec3(5.2, 5.0, 13.0);
	
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

//struct Bone {
//	int ID = 0;
//	std::string name = "";
//	std::vector<Bone> children = {};
//
//	glm::mat4 offset;	//反矩阵
//};

//struct BoneTransformTrack {
//	std::vector<float> positionTimestamps = {};
//	std::vector<float> rotationTimestamps = {};
//	std::vector<float> scaleTimestamps = {};
//
//	std::vector<glm::vec3> positions = {};
//	std::vector<glm::quat> rotations = {};
//	std::vector<glm::vec3> scales = {};
//};

//struct Animation {
//	
//	float duration = 0.0f;
//	float ticksPerSecond = 1.0f;
//	std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
//};

/* 弹簧模拟变数 */
//空气吹动力
int windBlowing = 0;
//空气力大小
int windForceScale = 15;
Vec3 windStartPos;
Vec3 windDir;
Vec3 wind;
//布料变数
Vec3 clothPos(2, 8, -5);
Vec2 clothSize(4, 4);
Cloth cloth(clothPos, clothSize);
//地面变数
//球变数
//重力
Vec3 gravity(0.0, 9.8 / cloth.iterationFreq, 0.0);



bool readSkeleton(Bone &boneOutput, aiNode *node, std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfoTable) {

	if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end()) {
		boneOutput.setName(node->mName.C_Str());
		boneOutput.setId(boneInfoTable[boneOutput.getName()].first);
		boneOutput.setOffset(boneInfoTable[boneOutput.getName()].second);

		for (int i = 0; i < node->mNumChildren; i++) {
			Bone child;
			readSkeleton(child, node->mChildren[i], boneInfoTable);
			//boneOutput.children.push_back(child);
			boneOutput.children_.push_back(child);
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

	//加载顶点，法线，uv坐标
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

		//取得模型坐标用变量
		Vertex vertex;
		//空的顶点
		glm::vec3 vector;

		//取得顶点
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.setPosition(vector);

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.setNormal(vector);

		glm::vec2 vec;

		//注意这里就只有一个纹理，需要的话需要动态设定
		vec.x = mesh->mTextureCoords[0][i].x;
		vec.y = mesh->mTextureCoords[0][i].y;
		vertex.setTexture(vec);

		//初始化骨骼ID
		vertex.setBoneIds(glm::ivec4(0));
		vertex.setBoneWeights(glm::vec4(0.0f));

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
				verticesOutput[id].setBoneIds_x(i);
				verticesOutput[id].setBoneWeights_x(weight);
				break;
			case 2:
				verticesOutput[id].setBoneIds_y(i);
				verticesOutput[id].setBoneWeights_y(weight);
				break;
			case 3:
				verticesOutput[id].setBoneIds_z(i);
				verticesOutput[id].setBoneWeights_z(weight);
				break;
			case 4:
				verticesOutput[id].setBoneIds_w(i);
				verticesOutput[id].setBoneWeights_w(weight);
				break;

			default:
				break;
			}
		}
	}

	//让所有权重的和为1
	for (unsigned int i = 0; i < verticesOutput.size(); i++) {
		//取得所有顶点数组里面的骨骼权重
		glm::vec4 &boneWeight = verticesOutput[i].getBoneWieghts();

		float totalWeight = boneWeight.x + boneWeight.y + boneWeight.z + boneWeight.w;
		if (totalWeight > 0.0f) {
			verticesOutput[i].setBoneWeights(glm::vec4(
				boneWeight.x / totalWeight,
				boneWeight.y / totalWeight,
				boneWeight.z / totalWeight,
				boneWeight.w / totalWeight)
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
		animation.ticksPerSecond_ = anim->mTicksPerSecond;
	}
	else {
		animation.ticksPerSecond_ = 1;
	}

	//动画间隔时间
	animation.duration_ = anim->mDuration * anim->mTicksPerSecond;
	//ticksPerSecond 时间单位
	animation.boneTransforms_ = {};

	/* 多重动画修复 */
	//是否为FBX模型
	bool checkAssimpFbx = false;
	//储存AssimpFbx动画的名称
	string assimpFbxStr;
	//存储AssimpFbx的动画序列
	vector<BoneTransformTrack> assimpFbxVector;
	/* 多重动画修复 */

	for (int i = 0; i < anim->mNumChannels; i++) {
		aiNodeAnim *channel = anim->mChannels[i];
		BoneTransformTrack track;

		for (int j = 0; j < channel->mNumPositionKeys; j++) {
			track.positionTimestamps_.push_back(channel->mPositionKeys[j].mTime);
			track.positions_.push_back(assimpToGlmVec3(channel->mPositionKeys[j].mValue));
		}
		for (int j = 0; j < channel->mNumRotationKeys; j++) {
			track.rotationTimestamps_.push_back(channel->mRotationKeys[j].mTime);
			track.rotations_.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));
		}
		//报错
		for (int j = 0; j < channel->mNumScalingKeys; j++) {
			track.scaleTimestamps_.push_back(channel->mScalingKeys[j].mTime);
			track.scales_.push_back(assimpToGlmVec3(channel->mScalingKeys[j].mValue));
		}
		

		/* 多重动画修复 */
		string nName(channel->mNodeName.C_Str());
		string::size_type ret = nName.find("_$AssimpFbx$_");
		if (ret != string::npos)
		{
			checkAssimpFbx = true;
			assimpFbxStr = nName.substr(0, ret);
			assimpFbxVector.push_back(track);
		}
		else {
			if (checkAssimpFbx) 
			{
				checkAssimpFbx = false;
				BoneTransformTrack outTrack;
				for (int i = 0; i < assimpFbxVector.size(); i++) {
					BoneTransformTrack item = assimpFbxVector[i];
					if (item.getPositions().size() > 1) {
						outTrack.setPositionTimestamps(item.getPositionTimestamps());
						outTrack.setPosition(item.getPositions());
					}
					if (item.getRotations().size() > 1) {
						outTrack.setRotationTimestamps(item.getRotationTimestamps());
						outTrack.setRotation(item.getRotations());
					}
					if (item.getScales().size() > 1) {
						outTrack.setScaleTimestamps(item.getScaleTimetamps());
						outTrack.setScale(item.getScales());
					}
				}
				std::cout << "loadAnimation() animation FBX=" << assimpFbxStr << std::endl;
				animation.boneTransforms_[assimpFbxStr] = outTrack;
			}
			animation.boneTransforms_[channel->mNodeName.C_Str()] = track;			
			std::cout << "loadAnimation() animation = " << channel->mNodeName.C_Str() << std::endl;
			/* 多重动画修复 */
		}
	}
}

//渲染
unsigned int createVertexArray(std::vector<Vertex> &vertices, std::vector<unsigned int> indices) {
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ebo = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) *vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	//VAO的通道号码(0~15) 三个长度为一个单位 是否需要正规化到正负1之间 下一个坐标要跳过多少 下一个信息的偏移量
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position_));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal_));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texture_));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneIds_));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneWeights_));

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

	while (dt > times[segment]) {
		segment++;
		if (segment >= times.size()) {
			segment--;
			break;
		}
	}

	float start = times[segment - 1];
	float end = times[segment];
	float frac = (dt - start) / (end - start);

	return{ segment, frac };
}

//得到当前姿势
void getPose(Animation &animation, Bone &skeleton, float dt, std::vector<glm::mat4> &output, glm::mat4 &parentTransform, glm::mat4 &globalInverseTransform) {
	BoneTransformTrack &boneTransformTrack = animation.boneTransforms_[skeleton.getName()];

	/* 多重动画修复 */
	if (boneTransformTrack.getPositions().size() == 0 || boneTransformTrack.getRotations().size() == 0 || boneTransformTrack.getScales().size() == 0)
		return;
	std::cout << "getPose() bone =" << skeleton.getName() << std::endl;
	/* 多重动画修复 */

	//余数
	dt = fmod(dt, animation.duration_);
	std::pair<unsigned int, float>fp;

	//实时顶点位置
	fp = getTimeFraction(boneTransformTrack.positionTimestamps_, dt);
	glm::vec3 position1 = boneTransformTrack.positions_[fp.first - 1];
	glm::vec3 position2 = boneTransformTrack.positions_[fp.first];
	glm::vec3 position = glm::mix(position1, position2, fp.second);

	//实时顶点旋转
	fp = getTimeFraction(boneTransformTrack.rotationTimestamps_, dt);
	glm::quat rotation1 = boneTransformTrack.rotations_[fp.first - 1];
	glm::quat rotation2 = boneTransformTrack.rotations_[fp.first];
	glm::quat rotation = glm::slerp(rotation1, rotation2, fp.second);

	//实时顶点缩放
	fp = getTimeFraction(boneTransformTrack.scaleTimestamps_, dt);
	glm::vec3 scale1 = boneTransformTrack.scales_[fp.first - 1];
	glm::vec3 scale2 = boneTransformTrack.scales_[fp.first];
	glm::vec3 scale = glm::mix(scale1, scale2, fp.second);

	glm::mat4 positionMat;
	glm::mat4 scaleMat;

	//计算本地变换
	positionMat = glm::translate(positionMat, position);
	glm::mat4 rotationMat = glm::toMat4(rotation);
	scaleMat = glm::scale(scaleMat, scale);

	//模型本体(local)变换
	glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
	//模型整体变换 复矩阵
	glm::mat4 globaTransform = parentTransform * localTransform;

	output[skeleton.Id_] = globalInverseTransform * globaTransform * skeleton.offset_;

	//更新子骨骼的数组
	for (Bone &child : skeleton.getChildren()) {
		getPose(animation, child, dt, output, globaTransform, globalInverseTransform);
	}
}

//------------------------------animation end------------------------

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
	GLFWwindow *window = glfwCreateWindow(1280, 720, "GameDemo", NULL, NULL);
	//注册窗口大小监听
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//实例化DisplayManager
	DisplayManager *myDisplayManager = new DisplayManager();
	//使用DisplayManager下的CreatManager
	myDisplayManager->CreatManager(window);
#pragma endregion

	//------------------------------animation start------------------------
	bool isRunning = true;

	//animation数组实例化
	Animation animation;
	//Bone骨骼数组实例化
	Bone skeleton;
	Vertex vertex;
	//实例化加载Assimp
	AnimaModelLoader animaModelLoader;
	animaModelLoader.loadAssimpScene("res/boss_lan.FBX");
	//animaModelLoader.loadAssimpScene("res/model.dae");

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("res/boss_lan.FBX", aiProcess_Triangulate);
	//const aiScene* scene = importer.ReadFile("res/model.dae", aiProcess_Triangulate);
	loadAnimation(scene, animation);

	//vao
	unsigned int vao = 0;
	//图片初始化
	unsigned int diffuseTexture;
	diffuseTexture = createTexture("res/boss_lan.jpg");
	vao = createVertexArray(animaModelLoader.getVertices(), animaModelLoader.getIndices());

	glm::mat4 identity;
	std::vector<glm::mat4> currentPose = {};
	currentPose.resize(animaModelLoader.getbBoneCount(), identity);

	//加载shader
	unsigned int shader = createShader(vertexShaderSource, fragmentShaderSource);

	//shader变量的设定和链接
	//create camera
	unsigned int viewMatrixLocation = glGetUniformLocation(shader, "view_Matrix");
	unsigned int projectionMatrixLocation = glGetUniformLocation(shader, "projection_Matrix");
	//modelMatrix变换
	unsigned int modelMatrixLocation = glGetUniformLocation(shader, "model_matrix");
	unsigned int boneMatricesLocation = glGetUniformLocation(shader, "bone_transforms");
	unsigned int textureLocation = glGetUniformLocation(shader, "diff_texture");

	//投影矩阵(projectionMatrix)
	//glm::mat4 projectionMatrix = glm::perspective(70.0f, (float)windowWidth / windowHeight, 0.1f, 1000.0f);
	//观察矩阵(viewMatrix)
	//glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.2f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1, 0));
	//投影矩阵 + 观察矩阵 TODO
	//glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	/** 骨骼模型位置，旋转，大小 **/
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(5.0f, 10.0f, 10.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

	//------------------------------animation end------------------------

	/* 布料模拟 */
	//布料绘制
	ClothRender clothRender(&cloth);
	Vec3 initForce(10.0, 40.0, 20.0);
	Vec3 testForce(0.0, 0.0, 0.2);
	cloth.addForce(initForce);

	/* 模型&地面 */
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
	//rawModel  modelTexture
	TexturedModel texturedModel(model, texture);
	//加载模型
	Entity entity(texturedModel, glm::vec3(30, 0, 5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	//加载灯光
	Light light(glm::vec3(400, 400, 200), glm::vec3(1, 1, 1));
	//加载地面
	Terrain terrain(0, 0, loader, ModelTexture(loader.loadTexture("grassy2")));
	Terrain terrain2(100, 10, loader, ModelTexture(loader.loadTexture("grassy3")));

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{

		//transformation
		entity.increasePosition(glm::vec3(0.0f, 0.0f, 0.0f));
		//rotation
		entity.increaseRotation(glm::vec3(0.0f, 0.0001f, 0.0f));

		masterRenderer.processTerrain(terrain2);
		masterRenderer.processTerrain(terrain);
		masterRenderer.processEntity(entity);
		masterRenderer.render(light, camera);
		masterRenderer.cleanUp();
		camera.move();

		/* 布料 */
		for (int i = 0; i < cloth.iterationFreq; i++) {
			cloth.computeForce(TIME_STEP, gravity);
			cloth.integrate(AIR_FRICTION, TIME_STEP);
			cloth.addForce(testForce);
		}
		cloth.computeNormal();
		clothRender.flush();

		//------------------------------animation start------------------------
		//取得当前程序运行时间
		float elapsedTime = glfwGetTime() * 50 + 0;

		float dAngle = 0.1;

		//modelMatrix = glm::rotate(modelMatrix, dAngle, glm::vec3(0, 0, 1));

		//(32010 / 30)
		//elapsedTime = (int)elapsedTime + 760 % (32010 / 30);
		//std::cout << "Time: " << elapsedTime << std::endl;
		//elapsedTime = elapsedTime < 1.0f ? 1.0f + 760.0f : elapsedTime;
		//std::cout << "Time: " << elapsedTime << std::endl;

		elapsedTime = elapsedTime+700;
		if (elapsedTime > 800)
			elapsedTime = 34;
		std::cout << "Time: " << elapsedTime << std::endl;


		getPose(animation, animaModelLoader.getSkeleton(), elapsedTime, currentPose, identity, animaModelLoader.getGlobalInverseTransform());

		glUseProgram(shader);
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(masterRenderer.getProjectionMatrix()));

		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(boneMatricesLocation, animaModelLoader.getbBoneCount(), GL_FALSE, glm::value_ptr(currentPose[0]));
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(textureLocation, 0);

		glDrawElements(GL_TRIANGLES, animaModelLoader.getIndices().size(), GL_UNSIGNED_INT, 0);

		//------------------------------animation end------------------------

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
