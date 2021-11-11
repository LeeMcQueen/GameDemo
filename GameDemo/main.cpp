#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
#include <windows.h>

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
#include "TerrainTexture.h"
#include "TerrainTexturePack.h"
#include "Player.h"
#include "Maths.h"
#include "WaterFrameBuffers.h"
#include "GuiTexture.h"
#include "GuiRenderer.h"
#include "Vertex.h"
#include "Bone.h"
#include "BoneTransformTrack.h"
#include "Animation.h"
#include "Cloth.h" 
#include "Rigid.h"
#include "Program.h"
#include "Display.h"
#include "Grasses.h"
#include "ShadowFrameBuffer.h"

//extern "C" {
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

#define AIR_FRICTION 0.02
#define TIME_STEP 0.01

#pragma region 骨骼动画vertexShader
const char* vertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec3 position; 
	layout (location = 1) in vec3 normal;
	layout (location = 2) in vec2 texture;
	layout (location = 3) in vec4 boneIds;
	layout (location = 4) in vec4 boneWeights;
	layout (location = 5) in vec3 tangent;
	layout (location = 6) in vec3 bitangent;

	uniform mat4 bone_transforms[50];		//jointTransForms[MAX_JOINTS]
	uniform mat4 view_Matrix;
	uniform mat4 projection_Matrix;
	uniform mat4 model_matrix;
	
	uniform vec3 lightPosition;
	uniform vec3 cameraPosition;

	out vec2 TexCoords;
	out vec4 bw;
	out vec3 TangentLightPos;
	out vec3 TangentViewPos;
	out vec3 TangentFragPos;
	out vec3 v_position;
	out mat3 v_tangentBasis;
	out vec3 v_cameraPosition;

	out vec3 reflectedVector;

	void main()
	{
		bw = vec4(0);
		if(int(boneIds.x) == 1)
		bw.z = boneIds.x;
		mat4 boneTransform  =  mat4(0.0);
		boneTransform += bone_transforms[int(boneIds.x)] * boneWeights.x;
		boneTransform += bone_transforms[int(boneIds.y)] * boneWeights.y;
		boneTransform += bone_transforms[int(boneIds.z)] * boneWeights.z;
		boneTransform += bone_transforms[int(boneIds.w)] * boneWeights.w;

		vec4 pos = boneTransform * vec4(position, 1.0);
		gl_Position = projection_Matrix * view_Matrix * model_matrix * pos;

		v_position = vec3(model_matrix * pos);
		TexCoords = texture;
		v_cameraPosition = cameraPosition;

		vec3 v_normal = mat3(transpose(inverse(model_matrix * boneTransform))) * normal;
		v_normal = normalize(v_normal);

		v_tangentBasis = mat3(transpose(inverse(model_matrix * boneTransform))) * mat3(tangent, bitangent, normal);

		vec3 T = normalize(v_normal * tangent);
		vec3 B = normalize(v_normal * bitangent);
		vec3 N = normalize(v_normal * normal);
		mat3 TBN = transpose(mat3(T, B, N));

		TangentLightPos = TBN * lightPosition;
		TangentViewPos = TBN * cameraPosition;
		TangentFragPos = TBN * v_position;

		vec3 viewVector = normalize(v_position.xyz - cameraPosition);
		reflectedVector = reflect(viewVector, v_normal);
	}
	)";
#pragma endregion

#pragma region 骨骼动画fragmentShader
const char* fragmentShaderSource = R"(
	#version 330 core

	in vec2 TexCoords;
	in vec4 bw;
	in vec3 TangentLightPos;
	in vec3 TangentViewPos;
	in vec3 TangentFragPos;
	in vec3	v_position;
	in mat3 v_tangentBasis;
	in vec3 v_cameraPosition;

	in vec3 reflectedVector;

	out vec4 out_Colour;

	uniform sampler2D diffTexture;
	uniform sampler2D emissionTexture;
	uniform sampler2D albedoTexture;
	uniform sampler2D normalTexture;
	uniform sampler2D metallicTexture;
	uniform sampler2D roughnessTexture;
	uniform sampler2D aoTexture;
	uniform samplerCube enviroMap;
	uniform float time;
	
	const float PI = 3.14159265359;
	const vec3 Fdielectric = vec3(5.0);
	const float Epsilon = 1.0;

	//--------------------------------------------------------------
	float ndfGGX(float cosLh, float roughness)
	{
		float alpha   = roughness * roughness;
		float alphaSq = alpha * alpha;

		float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
		return alphaSq / (PI * denom * denom);
	}
	float gaSchlickG1(float cosTheta, float k)
	{
		return cosTheta / (cosTheta * (1.0 - k) + k);
	}
	float gaSchlickGGX(float cosLi, float cosLo, float roughness)
	{
		float r = roughness + 1.0;
		float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
		return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
	}
	vec3 fresnelSchlick(vec3 F0, float cosTheta)
	{
		return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
	}
	//--------------------------------------------------------------

	void main()
	{
		vec3 albedo     = pow(texture(albedoTexture, TexCoords).rgb, vec3(2.2));
		float metallic  = texture(metallicTexture, TexCoords).r;
		float roughness = texture(roughnessTexture, TexCoords).r;
		float ao        = texture(aoTexture, TexCoords).r;

		vec3 Lo = normalize(v_cameraPosition - v_position);

		vec3 N = normalize(2.0 * texture(normalTexture, TexCoords).rgb - 1.0);
		N = normalize(v_tangentBasis * N);

		float cosLo = max(0.0, dot(N, Lo));
		vec3 Lr = 2.0 * cosLo * N - Lo;
		vec3 F0 = mix(Fdielectric, albedo, metallic);	
		vec3 directLighting = vec3(0.0);

		vec3 Li = -normalize(vec3(-1.0f, 0.0f, 0.0f));
		vec3 Lradiance = vec3(1.0);

		vec3 Lh = normalize(Li + Lo);
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));
		vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		float D = ndfGGX(cosLh, roughness);
		float G = gaSchlickGGX(cosLi, cosLo, roughness);
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);
		vec3 diffuseBRDF = kd * albedo;
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
		directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;

		vec3 normal = texture(normalTexture, TexCoords).rgb;
		normal = normalize(normal * 2.0 - 1.0);

		vec3 lightDir = normalize(TangentLightPos - TangentViewPos);
		float diff = max(dot(normal, lightDir), 0.3);

		vec3 dCol = diff * texture(diffTexture, TexCoords).rgb; 

		vec3 emissionTexture = texture(emissionTexture, TexCoords + vec2(0.0, time * 0.001)).rgb;
		emissionTexture = emissionTexture * (sin(time) * 0.001 + 0.1) * 5.0;
		
		out_Colour = vec4(dCol + emissionTexture + directLighting, 1.0f);

		vec4 reflectedColour = texture(enviroMap, reflectedVector);
		out_Colour = mix(out_Colour, reflectedColour, 0.2);
	}
	)";
#pragma endregion

#pragma region 布料变量设定
/* 弹簧模拟变数 */
//空气吹动力
int windBlowing = 0;
//空气力大小
int windForceScale = 15;
Vec3 windStartPos;
Vec3 windDir;
Vec3 wind;
//布料变数
Vec3 clothPos(140.0f, 35.0f, 50.0f);
Vec2 clothSize(3, 4);
Cloth cloth(clothPos, clothSize);
//TODO
//地面变数
//球变数
//重力
Vec3 gravity(0.0, 9.8 / cloth.iterationFreq, 0.0);
#pragma endregion

#pragma region 加载动画信息
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
#pragma endregion

#pragma region 骨骼动画渲染
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
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent_));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent_));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
	return vao;
}
#pragma endregion

#pragma region 时间处理
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
#pragma endregion

#pragma region 骨骼动画得到当前姿势
void getPose(Animation &animation, Bone &skeleton, float dt, std::vector<glm::mat4> &output, glm::mat4 &parentTransform, glm::mat4 &globalInverseTransform) {
	BoneTransformTrack &boneTransformTrack = animation.boneTransforms_[skeleton.getName()];

	/* 多重动画修复 */
	if (boneTransformTrack.getPositions().size() == 0 || boneTransformTrack.getRotations().size() == 0 || boneTransformTrack.getScales().size() == 0)
		return;
	//std::cout << "getPose() bone =" << skeleton.getName() << std::endl;
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
#pragma endregion

//窗口大小变换监听
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

#pragma region 创建窗口
	//初始化glfw
	glfwInit();
	//大版本3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//小版本3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//使用Opengl核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(1280, 720, "GameDemo", NULL, NULL);
	//注册窗口大小监听
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//实例化DisplayManager
	DisplayManager displayManager;
	//使用DisplayManager下的CreatManager
	displayManager.CreatManager(window);
#pragma endregion

#pragma region 骨骼动画设定
	//骨骼动画环境反射贴图
	std::vector<std::string> TEXTURE = {
		"res/right.png",
		"res/left.png",
		"res/top.png",
		"res/bottom.png",
		"res/back.png",
		"res/front.png"
	};
	bool isRunning = true;
	//实例化加载工具
	Loader loader;
	//animation数组实例化
	Animation animation;
	//Bone骨骼数组实例化
	Bone skeleton;
	//顶点数组实例化
	Vertex vertex;
	//实例化加载Assimp
	AnimaModelLoader animaModelLoader;
	//骨骼模型位置，旋转，大小
	glm::mat4 skeletonModelMatrix;
	//骨骼模型开始时间
	float RunStartTime = 865.0f;
	float RunEndTime = 888.0f;
	float idleStartTime = 805.0f;
	float idleEndTime = 856.0f;

	animaModelLoader.loadAssimpScene("res/warhummer.FBX");

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("res/boss_lan.FBX", aiProcess_Triangulate);
	loadAnimation(scene, animation);

	unsigned int vao = 0;
	//图片初始化
	unsigned int diffuseTexture;
	//diffuseTexture = loader.loadTexture("boss_lan");old-metal-slats1_albedo
	diffuseTexture = loader.loadTexture("old-metal-slats1_albedo");
	unsigned int emissionTexture;
	emissionTexture = loader.loadTexture("boss_green");
	unsigned int albedoTexture;
	albedoTexture = loader.loadTexture("old-metal-slats1_albedo");
	unsigned int normalTexture;
	normalTexture = loader.loadTexture("boss_normal");
	unsigned int metallicTexture;
	metallicTexture = loader.loadTexture("old-metal-slats1_metallic");
	unsigned int roughnessTexture;
	roughnessTexture = loader.loadTexture("old-metal-slats1_roughness");
	unsigned int aoTexture;
	aoTexture = loader.loadTexture("old-metal-slats1_ao");
	unsigned int cubeMap;
	cubeMap = loader.loadCubeMap(TEXTURE);

	//创建骨骼动画的顶点数组对象
	vao = createVertexArray(animaModelLoader.getVertices(), animaModelLoader.getIndices());

	//得到动画个数，然后生成相应大小的list（空的matrix，之后用getpose得到对应的matrix）
	glm::mat4 identity;
	std::vector<glm::mat4> currentPose = {};
	currentPose.resize(animaModelLoader.getbBoneCount(), identity);

	//加载shader
	unsigned int shader = createShader(vertexShaderSource, fragmentShaderSource);

	//shader变量的设定和链接
	unsigned int viewMatrixLocation = glGetUniformLocation(shader, "view_Matrix");
	unsigned int projectionMatrixLocation = glGetUniformLocation(shader, "projection_Matrix");
	//modelMatrix变换
	unsigned int modelMatrixLocation = glGetUniformLocation(shader, "model_matrix");
	unsigned int boneMatricesLocation = glGetUniformLocation(shader, "bone_transforms");
	unsigned int textureLocation = glGetUniformLocation(shader, "diffTexture");
	unsigned int emissionLocation = glGetUniformLocation(shader, "emissionTexture");
	unsigned int albedoLocation = glGetUniformLocation(shader, "albedoTexture");
	unsigned int normalLocation = glGetUniformLocation(shader, "normalTexture");
	unsigned int metallicLocation = glGetUniformLocation(shader, "metallicTexture");
	unsigned int roughnessLocation = glGetUniformLocation(shader, "roughnessTexture");
	unsigned int aoLocation = glGetUniformLocation(shader, "aoTexture");
	unsigned int cubeMapLocation = glGetUniformLocation(shader, "enviroMap");
	unsigned int cameraPositionLocation = glGetUniformLocation(shader, "cameraPosition");
	unsigned int lightPositionLocation = glGetUniformLocation(shader, "lightPosition");
	unsigned int timeLocation = glGetUniformLocation(shader, "time");
#pragma endregion

#pragma region 草地
	//草地
	Grasses grasses;
	//计算shader插值ID
	unsigned int cameraUniformBuffer = 0;
	unsigned int lightCameraUniformBuffer = 0;
	//游戏进行时间（草地用）
	using DeltaDuration = std::chrono::duration<float, std::milli>;

	glGenBuffers(1, &cameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUniformBuffer);

	glGenBuffers(1, &lightCameraUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lightCameraUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, lightCameraUniformBuffer);

	DeltaDuration deltaTime;
	std::chrono::steady_clock::time_point lastFrame;
	lastFrame = std::chrono::steady_clock::now();

	grasses.init();
#pragma endregion

	//主角控制
	Player player(glm::vec3(100.0f, 0.0f, 100.0f), glm::vec3(-90.0f, 0.0f, 180.0f), glm::vec3(8.0f, 8.0f, 8.0f));
	//实例化相机
	Camera camera;
	//水面FBOs
	WaterFrameBuffers fbos;
	//阴影FBOs
	ShadowFrameBuffer shadowFrameBuffer;
	//实例化渲染器 透视投影
	MasterRenderer masterRenderer(loader, fbos, shadowFrameBuffer, true);
	//实例化渲染器 正交投影
	MasterRenderer masterRendererOrtho(loader, fbos, shadowFrameBuffer, false);
	//实例化加载OBJ
	OBJLoader objloader;
	//GuiShader
	GuiShader guiShader;
	//Gui渲染启动
	GuiRenderer guiRenderer(guiShader, loader);

	//加载主角模型顶点信息
	RawModel model = objloader.loadObjModel("person");
	//使用纹理文件名加载纹理
	ModelTexture texture(loader.loadTexture("playerTexture"));
	texture.setShineDamer(100.0f);
	texture.setReflectivity(1.0f);
	TexturedModel texturedModel(model, texture);
	//加载杂草模型
	TexturedModel fernModel = TexturedModel(OBJLoader::loadObjModel("fern"), ModelTexture(loader.loadTexture("fern"), true, false));
	TexturedModel treeModel = TexturedModel(OBJLoader::loadObjModel("tree"), ModelTexture(loader.loadTexture("tree"), true, false));

	//加载模型
	Entity entity(texturedModel, glm::vec3(30, 0, 5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	Entity fern(fernModel, glm::vec3(40, 0, 10), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	Entity tree(treeModel, glm::vec3(80, 0, 50), glm::vec3(0, 0, 0), glm::vec3(10, 10, 10));
	Entity underTree(treeModel, glm::vec3(80, -35, 80), glm::vec3(0, 0, 0), glm::vec3(10, 10, 10));

	//加载灯光
	Light light(glm::vec3(400, 400, 200), glm::vec3(1, 1, 1));

	//加载地面 混合纹理
	TerrainTexture backgroundTexture = TerrainTexture(loader.loadTexture("grassy2"));
	TerrainTexture rTexture = TerrainTexture(loader.loadTexture("dirt"));
	TerrainTexture gTexture = TerrainTexture(loader.loadTexture("pinkFlowers"));
	TerrainTexture bTexture = TerrainTexture(loader.loadTexture("path"));
	TerrainTexturePack terrainTexturePack = TerrainTexturePack(backgroundTexture, rTexture, gTexture, bTexture);
	TerrainTexture blendMap = TerrainTexture(loader.loadTexture("blendMap"));

	//地面类初始化
	Terrain terrain = Terrain(0, 0, loader, terrainTexturePack, blendMap);
	//水面
	WaterTile waterTile = WaterTile(15.0, 20.0, -7.0, loader);

	//Gui列表
	std::vector<GuiTexture> guiTextures;
	GuiTexture shadow = GuiTexture(shadowFrameBuffer.getShadowMap(), glm::vec2(-1, -1), glm::vec2(0.2, 0.2));
	GuiTexture reflection = GuiTexture(fbos.getReflectionTexture(), glm::vec2(-1, 1), glm::vec2(0.2, 0.2));
	GuiTexture refraction = GuiTexture(fbos.getRefractionTexture(), glm::vec2(1, 1), glm::vec2(0.2, 0.2));
	guiTextures.push_back(shadow);
	guiTextures.push_back(reflection);
	guiTextures.push_back(refraction);

	//布料绘制
	Vec3 initForce(10.0, 40.0, 20.0);
	Vec3 normalForce(0.0, 0.0, 0.5);
	ClothRender clothRender(&cloth, masterRenderer);
	cloth.addForce(initForce);

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		player.move(terrain);
		camera.move(player.getPosition(), player.getRotation());

		shadowFrameBuffer.bindShadowFrameBuffer();
		masterRendererOrtho.processEntity(tree);
		masterRendererOrtho.processEntity(underTree);
		auto shadowMapCamera = camera;
		shadowMapCamera.setviewDirection(glm::vec3(400.0f, 400.0f, 200.0f));
		glm::mat4 terrainLightViewMatrix = shadowMapCamera.getViewMatrix();
		masterRendererOrtho.render(light, shadowMapCamera, glm::vec4(0.0f, -1.0f, 0.0f, waterTile.getHeight()), terrainLightViewMatrix);
		#pragma region 骨骼动画主循环
		//移动 得到实时的变换matrix
		skeletonModelMatrix = Maths::createTransformationMatrix(player.getPosition(), player.getRotation(), player.getScale());

		//得到游戏每循坏一次所需时间(32010 / 30)
		displayManager.setDeltaTime((displayManager.getCurrentFrameTime() - displayManager.getLastFrameTime()) * 30);
		displayManager.setLastFrameTime(displayManager.getCurrentFrameTime());

		idleStartTime = idleStartTime + displayManager.getDeltaTime();

		//骨骼动画控制
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
			RunStartTime = RunStartTime + displayManager.getDeltaTime();
			if (RunStartTime > RunEndTime)
				RunStartTime = 865.0f;
			//std::cout << RUNelapsedTime << std::endl;
			getPose(animation, animaModelLoader.getSkeleton(), RunStartTime, currentPose, identity, animaModelLoader.getGlobalInverseTransform());
		}
		else {
			if (idleStartTime > idleEndTime)
				idleStartTime = 805.0f;
			//std::cout << RUNelapsedTime << std::endl;
			getPose(animation, animaModelLoader.getSkeleton(), idleStartTime, currentPose, identity, animaModelLoader.getGlobalInverseTransform());
		}
		//骨骼动画shader传值
		glUseProgram(shader);
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(shadowMapCamera.getViewMatrix()));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(masterRenderer.getProjectionMatrix(false)));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(skeletonModelMatrix));
		glUniformMatrix4fv(boneMatricesLocation, animaModelLoader.getbBoneCount(), GL_FALSE, glm::value_ptr(currentPose[0]));
		glUniform3f(cameraPositionLocation, camera.getPosition().x, camera.getPosition().y + 30.0f, camera.getPosition().z + 40.0f);
		glUniform3f(lightPositionLocation, light.getPosition().x, light.getPosition().y, light.getPosition().z);
		glBindVertexArray(vao);
		//骨骼动画基础纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(textureLocation, 0);
		//骨骼动画运动纹理
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, emissionTexture);
		glUniform1i(emissionLocation, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, albedoTexture);
		glUniform1i(albedoLocation, 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glUniform1i(normalLocation, 3);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, metallicTexture);
		glUniform1i(metallicLocation, 4);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, roughnessTexture);
		glUniform1i(roughnessLocation, 5);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, aoTexture);
		glUniform1i(aoLocation, 6);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		glUniform1i(cubeMapLocation, 7);
		//骨骼动画运动纹理时间单位
		glUniform1f(timeLocation, idleStartTime);
		glDrawElements(GL_TRIANGLES, animaModelLoader.getIndices().size(), GL_UNSIGNED_INT, 0);
		#pragma endregion
		shadowFrameBuffer.unbindShadowFrameBuffer();

		glEnable(GL_CLIP_DISTANCE0);
		//水面反射buffer
		fbos.bindReflectionFrameBuffer();
		auto reflectionCamera = camera;
		reflectionCamera.setviewDirection(glm::vec3(0.0f, 30.0f, 40.0f));
		masterRenderer.render(light, reflectionCamera, glm::vec4(0.0f, 1.0f, 0.0f, -waterTile.getHeight() + 0.5f), terrainLightViewMatrix);
		fbos.unbindCurrentFrameBuffer();
		//水面折射buffer
		fbos.bindRefractionFrameBuffer();
		masterRenderer.processTerrain(terrain);
		masterRenderer.processEntity(underTree);
		masterRenderer.render(light, camera, glm::vec4(0.0f, -1.0f, 0.0f, waterTile.getHeight()), terrainLightViewMatrix);
		fbos.unbindCurrentFrameBuffer();
		glDisable(GL_CLIP_DISTANCE0);

		//加载
		masterRenderer.processTerrain(terrain);
		masterRenderer.processWater(waterTile);
		masterRenderer.processEntity(entity);
		masterRenderer.processEntity(tree);
		masterRenderer.processEntity(fern);
		masterRenderer.render(light, camera, glm::vec4(0.0f, -1.0f, 0.0f, 15.0f), terrainLightViewMatrix);
		guiRenderer.render(guiTextures);

#pragma region 草地主循环
		//草地渲染时间
		const auto current_time = std::chrono::steady_clock::now();
		deltaTime = current_time - lastFrame;
		lastFrame = current_time;
		//草地的观察矩阵&投影矩阵
		glm::mat4 grassViewMatrix = camera.getViewMatrix();
		glm::mat4 grassProjectionMatrix = masterRenderer.getProjectionMatrix(true);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &grassViewMatrix);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &grassProjectionMatrix);

		glm::mat4 lightViewMatrix = shadowMapCamera.getViewMatrix();
		glm::mat4 lightProjectionMatrix = masterRenderer.getProjectionMatrix(false);

		glBindBufferBase(GL_UNIFORM_BUFFER, 4, lightCameraUniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, lightCameraUniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &lightViewMatrix);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &lightProjectionMatrix);

		//草地更新
		grasses.update(deltaTime, shadowFrameBuffer);
		//草地渲染
		grasses.render();
#pragma endregion

#pragma region 布料主循环
		for (int i = 0; i < cloth.iterationFreq; i++) {
			cloth.computeForce(TIME_STEP, gravity);
			cloth.integrate(AIR_FRICTION, TIME_STEP);
			cloth.addForce(normalForce);
		}
		cloth.computeNormal();
		clothRender.flush(camera);
#pragma endregion

#pragma region 骨骼动画主循环
		//移动 得到实时的变换matrix
		skeletonModelMatrix = Maths::createTransformationMatrix(player.getPosition(), player.getRotation(), player.getScale());

		//得到游戏每循坏一次所需时间(32010 / 30)
		displayManager.setDeltaTime((displayManager.getCurrentFrameTime() - displayManager.getLastFrameTime()) * 30);
		displayManager.setLastFrameTime(displayManager.getCurrentFrameTime());

		idleStartTime = idleStartTime + displayManager.getDeltaTime();

		//骨骼动画控制
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
			RunStartTime = RunStartTime + displayManager.getDeltaTime();
			if (RunStartTime > RunEndTime)
				RunStartTime = 865.0f;
			//std::cout << RUNelapsedTime << std::endl;
			getPose(animation, animaModelLoader.getSkeleton(), RunStartTime, currentPose, identity, animaModelLoader.getGlobalInverseTransform());
		}
		else {
			if (idleStartTime > idleEndTime)
				idleStartTime = 805.0f;
			//std::cout << RUNelapsedTime << std::endl;
			getPose(animation, animaModelLoader.getSkeleton(), idleStartTime, currentPose, identity, animaModelLoader.getGlobalInverseTransform());
		}

		//骨骼动画shader传值
		glUseProgram(shader);
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(masterRenderer.getProjectionMatrix(true)));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(skeletonModelMatrix));
		glUniformMatrix4fv(boneMatricesLocation, animaModelLoader.getbBoneCount(), GL_FALSE, glm::value_ptr(currentPose[0]));
		glUniform3f(cameraPositionLocation, camera.getPosition().x, camera.getPosition().y + 30.0f, camera.getPosition().z + 40.0f);
		glUniform3f(lightPositionLocation, light.getPosition().x, light.getPosition().y, light.getPosition().z);
		glBindVertexArray(vao);
		//骨骼动画基础纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(textureLocation, 0);
		//骨骼动画运动纹理
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, emissionTexture);
		glUniform1i(emissionLocation, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, albedoTexture);
		glUniform1i(albedoLocation, 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glUniform1i(normalLocation, 3);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, metallicTexture);
		glUniform1i(metallicLocation, 4);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, roughnessTexture);
		glUniform1i(roughnessLocation, 5);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, aoTexture);
		glUniform1i(aoLocation, 6);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		glUniform1i(cubeMapLocation, 7);
		//骨骼动画运动纹理时间单位
		glUniform1f(timeLocation, idleStartTime);

		glDrawElements(GL_TRIANGLES, animaModelLoader.getIndices().size(), GL_UNSIGNED_INT, 0);
#pragma endregion

		//按下Esc就关闭窗口
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		//交换颜色缓冲区
		glfwSwapBuffers(window);
		//检查是否出发相关事件
		glfwPollEvents();
	}

	guiRenderer.cleanUp();
	masterRenderer.cleanUp();
	masterRendererOrtho.cleanUp();
	fbos.cleanUp();
	shadowFrameBuffer.cleanUp();

	//销毁GLFW
	glfwTerminate();

}