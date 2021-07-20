#include "AnimaModelLoader.h"

void AnimaModelLoader::loadAssimpScene(const char *filePath){

	//导入assimp加载模组
	Assimp::Importer importer;
	//顶点类
	std::vector<Vertex> vertices;
	//顶点总数
	std::vector<unsigned int> indices;
	//骨骼总数
	unsigned int boneCount;
	//Bone骨骼类
	Bone skeleton;
	//顶点类
	Vertex vertex;
	//通过ReadFile函数得到Scene
	const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	//Assimp加载成功判定
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::Assimp :" << importer.GetErrorString() << std::endl;
	}
	//Scene里的Mesh
	aiMesh* mesh = scene->mMeshes[0];

	globalInverseTransform_ = assimpToGlmMatrix(scene->mRootNode->mTransformation);
	setGlobaInverseTransform(glm::inverse(globalInverseTransform_));
	//加载骨骼动画模型
	loadAssimpModel(scene, mesh, vertices, indices, skeleton, boneCount);

	//数据封装进入容器内
	setVertices(vertices);
	setIndices(indices);
	setSkeleton(skeleton);
	setBoneCount(boneCount);

};

void AnimaModelLoader::loadAssimpModel(const aiScene *scene,
	aiMesh *mesh, 
	std::vector<Vertex> &verticesOutput, 
	std::vector<unsigned int> &indicesOutput, 
	Bone &skeletonOutput, 
	unsigned int &nBoneCount){
	//list初始化
	verticesOutput = {};
	indicesOutput = {};

	//取得模型坐标用变量
	Vertex vertex;
	//加载顶点，法线，uv坐标
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

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

	//循环骨骼数量得到()
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

	readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);

}

//a recursive function to read all bones and form skeleton
bool AnimaModelLoader::readSkeleton(Bone &boneOutput, aiNode *node, std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfoTable){

	if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end()) {
		boneOutput.setName(node->mName.C_Str());
		boneOutput.setId(boneInfoTable[boneOutput.getName()].first);
		boneOutput.setOffset(boneInfoTable[boneOutput.getName()].second);

		for (int i = 0; i < node->mNumChildren; i++) {
			Bone child;
			readSkeleton(child, node->mChildren[i], boneInfoTable);
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

//加载动画信息
void AnimaModelLoader::loadAnimation(const aiScene *scene, Animation &animation){

	//加载第一个动画,多个动画加载需要修改
	aiAnimation *anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0f) {
		animation.ticksPerSecond_ = anim->mTicksPerSecond;
	}
	else {
		animation.ticksPerSecond_ = 1;
	}

	animation.duration_ = anim->mDuration * anim->mTicksPerSecond;
	animation.boneTransforms_ = {};

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
		animation.boneTransforms_[channel->mNodeName.C_Str()] = track;
		//std::cout << "loadAnimation() animation = " << channel->mNodeName.C_Str() << std::endl;
	}
};
