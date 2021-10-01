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

	out vec2 tex_cord;
	out vec3 v_normal;
	out vec3 v_pos;
	out vec4 bw;

	uniform mat4 bone_transforms[50];		//jointTransForms[MAX_JOINTS]
	uniform mat4 view_Matrix;
	uniform mat4 projection_Matrix;
	uniform mat4 model_matrix;

	void main()
	{
		bw = vec4(0);
		if(int(boneIds.x) == 1)
		bw.z = boneIds.x;
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
#pragma endregion

#pragma region 骨骼动画fragmentShader
const char* fragmentShaderSource = R"(
	#version 330 core

	in vec2 tex_cord;
	in vec3 v_normal;
	in vec3 v_pos;
	in vec4 bw;

	out vec4 color;

	uniform sampler2D diff_texture;
	uniform sampler2D emission;
	uniform float time;

	vec3 lightPos = vec3(20.0, 55.0, 13.0);
	
	void main()
	{
		vec3 lightDir = normalize(lightPos - v_pos);
		float diff = max(dot(v_normal, lightDir), 0.2);

		vec3 dCol = diff * texture(diff_texture, tex_cord).rgb; 

		vec3 emission = texture(emission, tex_cord + vec2(0.0, time * 0.001)).rgb;
		emission = emission * (sin(time) * 0.001 + 0.5) * 2.0;

		color = vec4(dCol + emission, 1.0f);
	}
	)";
#pragma endregion

/* 弹簧模拟变数 */
//空气吹动力
int windBlowing = 0;
//空气力大小
int windForceScale = 15;
Vec3 windStartPos;
Vec3 windDir;
Vec3 wind;
//布料变数
Vec3 clothPos(20.0f, 20.0f, 30.0f);
Vec2 clothSize(3, 4);
Cloth cloth(clothPos, clothSize);
//TODO
//地面变数
//球变数
//重力
Vec3 gravity(0.0, 9.8 / cloth.iterationFreq, 0.0);

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//小版本3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
	animaModelLoader.loadAssimpScene("res/warhummer.FBX");

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("res/boss_lan.FBX", aiProcess_Triangulate);
	loadAnimation(scene, animation);

	unsigned int vao = 0;
	//图片初始化
	unsigned int diffuseTexture;
	diffuseTexture = loader.loadTexture("boss_lan");
	unsigned int emissionTexture;
	emissionTexture = loader.loadTexture("boss_green");
	
	//创建骨骼动画的顶点数组对象
	vao = createVertexArray(animaModelLoader.getVertices(), animaModelLoader.getIndices());

	//得到动画个数，然后生成相应大小的list（空的matrix，之后用getpose得到对应的matrix）
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
	unsigned int emissionLocation = glGetUniformLocation(shader, "emission");
	unsigned int timeLocation = glGetUniformLocation(shader, "time");
#pragma endregion

	//主角控制
	Player player(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(8.0f, 8.0f, 8.0f));
	//实例化相机
	Camera camera(player);
	//水面FBOs
	WaterFrameBuffers fbos;
	//实例化渲染器
	MasterRenderer masterRenderer(loader, fbos);
	//实例化加载OBJ
	OBJLoader objloader;
	//GuiShader
	GuiShader guiShader;
	//Gui渲染启动
	GuiRenderer guiRenderer(guiShader, loader);
	//草地
	Grasses grasses;
	//游戏进行时间（草地用）
	using DeltaDuration = std::chrono::duration<float, std::milli>;
	DeltaDuration delta_time_;
	std::chrono::steady_clock::time_point last_frame_;
	
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
	Entity tree(treeModel, glm::vec3(50, 0, 30), glm::vec3(0, 0, 0), glm::vec3(10, 10, 10));

	//加载灯光
	Light light(glm::vec3(400, 400, 200), glm::vec3(1, 1, 1));

	//加载地面 混合纹理
	TerrainTexture backgroundTexture = TerrainTexture(loader.loadTexture("grassy2"));
	TerrainTexture rTexture = TerrainTexture(loader.loadTexture("dirt"));
	TerrainTexture gTexture = TerrainTexture(loader.loadTexture("pinkFlowers"));
	TerrainTexture bTexture = TerrainTexture(loader.loadTexture("path"));
	TerrainTexturePack terrainTexturePack = TerrainTexturePack(
		backgroundTexture, rTexture, gTexture, bTexture);
	TerrainTexture blendMap = TerrainTexture(loader.loadTexture("blendMap"));

	//地面类初始化
	Terrain terrain = Terrain(-100, -100, loader, terrainTexturePack, blendMap);
	//水面
	WaterTile waterTile = WaterTile(-10, -15, -10, loader);

	//Gui列表
	std::vector<GuiTexture> guiTextures;
	GuiTexture reflection = GuiTexture(fbos.getReflectionTexture(), glm::vec2(-1, 1), glm::vec2(0.2, 0.2));
	GuiTexture refraction = GuiTexture(fbos.getRefractionTexture(), glm::vec2(1, 1), glm::vec2(0.2, 0.2));
	guiTextures.push_back(reflection);
	guiTextures.push_back(refraction);

	//布料绘制
	Vec3 initForce(10.0, 40.0, 20.0);
	Vec3 normalForce(0.0, 0.0, 0.5);
	ClothRender clothRender(&cloth, masterRenderer);
	cloth.addForce(initForce);

	//骨骼模型开始时间
	float RunStartTime = 865.0f;
	float RunEndTime = 888.0f;
	float idleStartTime = 805.0f;
	float idleEndTime = 856.0f;
	//骨骼模型位置，旋转，大小
	glm::mat4 modelMatrix;

	grasses.init();

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//OBJ模型的移动
		entity.increasePosition(glm::vec3(0.0f, 0.0f, 0.0f));
		//OBJ模型的旋转
		entity.increaseRotation(glm::vec3(0.0f, 0.01f, 0.0f));

		//水面反射buffer
		glEnable(GL_CLIP_DISTANCE0);
		fbos.bindReflectionFrameBuffer();
		auto reflectionCamera = camera;
		reflectionCamera.setviewDirection(glm::vec3(0.0f, -40.0f, 70.0f));
		masterRenderer.processEntity(entity);
		masterRenderer.processEntity(fern);
		masterRenderer.processEntity(tree);
		masterRenderer.processTerrain(terrain);
		masterRenderer.render(light, reflectionCamera, glm::vec4(0.0f, 1.0f, 0.0f, -waterTile.getHeight()));
		fbos.unbindCurrentFrameBuffer();
		//水面折射buffer
		fbos.bindRefractionFrameBuffer();
		masterRenderer.processEntity(entity);
		masterRenderer.processEntity(fern);
		masterRenderer.processEntity(tree);
		masterRenderer.processTerrain(terrain);
		masterRenderer.render(light, camera, glm::vec4(0.0f, -1.0f, 0.0f, waterTile.getHeight()));
		fbos.unbindCurrentFrameBuffer();
		glDisable(GL_CLIP_DISTANCE0);

		//加载
		//masterRenderer.processTerrain(terrain);
		//masterRenderer.processWater(waterTile);
		masterRenderer.processEntity(entity);
		masterRenderer.processEntity(tree);
		masterRenderer.processEntity(fern);
		masterRenderer.render(light, camera, glm::vec4(0.0f, -1.0f, 0.0f, 15.0f));
		guiRenderer.render(guiTextures);
		player.move();
		camera.move(player.getPosition(), player.getRotation(), player.getScale());

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
		//移动
		modelMatrix = Maths::createTransformationMatrix(player.getPosition(), player.getRotation(), player.getScale());

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
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(masterRenderer.getProjectionMatrix()));

		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(boneMatricesLocation, animaModelLoader.getbBoneCount(), GL_FALSE, glm::value_ptr(currentPose[0]));
		glBindVertexArray(vao);
		//骨骼动画基础纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(textureLocation, 0);
		//骨骼动画运动纹理
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, emissionTexture);
		glUniform1i(emissionLocation, 1);
		//骨骼动画运动纹理时间单位
		glUniform1f(timeLocation, idleStartTime);


		glDrawElements(GL_TRIANGLES, animaModelLoader.getIndices().size(), GL_UNSIGNED_INT, 0);
#pragma endregion

		const auto current_time = std::chrono::steady_clock::now();
		delta_time_ = current_time - last_frame_;
		grasses.update(delta_time_);
		grasses.render();

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
	fbos.cleanUp();

	//销毁GLFW
	glfwTerminate();

}