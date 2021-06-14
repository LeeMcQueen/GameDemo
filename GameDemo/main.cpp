
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

//------------------------------test------------------------
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <unordered_map>
//------------------------------test------------------------

//------------------------------test------------------------

struct Vertex {
	glm::vec3 position;	//顶点
	glm::vec3 normal;	//法线
	glm::vec2 uv;		//图片uv坐标
	glm::vec4 boneIds;	//骨骼ID(joint关节ID)
	glm::vec4 boneWeight;	//骨骼权重
};

struct Bone{
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
	std::vector<glm::quat> ratations = {};
	std::vector<glm::vec3> scales = {};
};

struct Animation {
	float duration = 0.0f;
	float ticksPerSecond = 1.0f;
	std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
};

//------------------------------test------------------------


//窗口大小变换监听
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}

int main() {

	//------------------------------test------------------------

	//使用assimp加载模型
	Assimp::Importer importer;
	const char* filePath = "res/model.dae";
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	//Assimp加载成功判定
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR::Assimp :" << importer.GetErrorString() << std::endl;
	}
	aiMesh* mesh = scene->mMeshes[0];
	//std::cout << "Test :" << mesh << std::endl;

	//变量初始化
	std::vector<Vertex> vertices = {};	//顶点数组
	std::vector<Vertex> indices = {};	//顶点顺序变量
	unsigned int boneCount = 0;
	Animation animation;	//animation数组实例化
	unsigned int vao = 0;
	Bone skeleton;	//Bone骨骼数组实例化
	unsigned int diffuseTexture;	//图片初始化

	glm::mat4 globaInverseTransform 
		//globaInverseTransform = scene->mRootNode->mTransformation


	//------------------------------test------------------------

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

	//start temporary data
	Loader loader;

	//camera
	Camera camera;

	//渲染器
	MasterRenderer masterRenderer;

	//load ObjModle form objloader function
	OBJLoader objloader;

	//load OBJModel 3 function
	/*RawModel model = myOBJLoader->tinyOBJLoader("stall");*/
	/*RawModel model = myOBJLoader->loadModel("stall");*/
	RawModel model = objloader.loadObjModel("person");

	//load texture use NAME
	ModelTexture texture(loader.loadTexture("playerTexture"));
	texture.setShineDamer(100.0f);
	texture.setReflectivity(1.0f);

	TexturedModel texturedModel(model, texture);

	//model load
	Entity entity(texturedModel, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
	//light load
	Light light(glm::vec3(400, 400, 200), glm::vec3(1, 1, 1));
	Terrain terrain(0, 0, loader, ModelTexture(loader.loadTexture("grassy2")));
	Terrain terrain2(100, 10, loader, ModelTexture(loader.loadTexture("grassy3")));

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//------------------------------test------------------------

		//------------------------------test------------------------

		//close window
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

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

		//交换颜色缓冲区
		glfwSwapBuffers(window);

		//检查是否出发相关事件
		glfwPollEvents();
	}

	//销毁GLFW
	glfwTerminate();

}

