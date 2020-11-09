//#include <iostream>
//
//#include "Mesh.h"
//#include "struct.h"
//
//const char* vertexShaderSource = R"(
//	#version 440 core
//	layout (location = 0) in vec3 position; 
//	layout (location = 1) in vec3 normal;
//	layout (location = 2) in vec2 uv;
//	layout (location = 3) in vec4 boneIds;
//	layout (location = 4) in vec4 boneWeights;
//	out vec2 tex_cord;
//	out vec3 v_normal;
//	out vec3 v_pos;
//	out vec4 bw;
//	uniform mat4 bone_transforms[50];
//	uniform mat4 view_projection_matrix;
//	uniform mat4 model_matrix;
//	void main()
//	{
//		bw = vec4(0);
//		if(int(boneIds.x) == 1)
//		bw.z = boneIds.x;
//		//boneWeights = normalize(boneWeights);
//		mat4 boneTransform  =  mat4(0.0);
//		boneTransform  +=    bone_transforms[int(boneIds.x)] * boneWeights.x;
//		boneTransform  +=    bone_transforms[int(boneIds.y)] * boneWeights.y;
//		boneTransform  +=    bone_transforms[int(boneIds.z)] * boneWeights.z;
//		boneTransform  +=    bone_transforms[int(boneIds.w)] * boneWeights.w;
//		vec4 pos =boneTransform * vec4(position, 1.0);
//		gl_Position = view_projection_matrix * model_matrix * pos;
//		v_pos = vec3(model_matrix * boneTransform * pos);
//		tex_cord = uv;
//		v_normal = mat3(transpose(inverse(model_matrix * boneTransform))) * normal;
//		v_normal = normalize(v_normal);
//	}
//	)";
//const char* fragmentShaderSource = R"(
//	#version 440 core
//	in vec2 tex_cord;
//	in vec3 v_normal;
//	in vec3 v_pos;
//	in vec4 bw;
//	out vec4 color;
//	uniform sampler2D diff_texture;
//	vec3 lightPos = vec3(0.2, 1.0, -3.0);
//	
//	void main()
//	{
//		vec3 lightDir = normalize(lightPos - v_pos);
//		float diff = max(dot(v_normal, lightDir), 0.2);
//		vec3 dCol = diff * texture(diff_texture, tex_cord).rgb; 
//		color = vec4(dCol, 1);
//	}
//	)";
//
//Mesh::Mesh()
//{}
//
//void Mesh::AmLoader(std::string fileName)
//{
//	Assimp::Importer importer;
//
//	const char* filePath = 0;
//
//	bool isRunning = true;
//
//	const aiScene* scene = importer.ReadFile(filePath,
//		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//	{
//		std::cout << "ERROR: [Mesh::AmLoader] :" << importer.GetErrorString() << "!" << std::endl;
//	}
//	aiMesh* mesh = scene->mMeshes[0];
//
//	glm::mat4 globalInverseTransform = assimpToGlmMatrix(scene->mRootNode->mTransformation);
//	globalInverseTransform = glm::inverse(globalInverseTransform);
//
//	loadModel(scene, mesh, vertices, indices, skeleton, boneCount);
//	loadAnimation(scene, animation);
//
//	vao = createVertexArray(vertices, indices);
//	diffuseTexture = createTexture("res/diffuse.png");
//
//	glm::mat4 identity(1.0);
//
//	//currentPose is held in this vector and uploaded to gpu as a matrix array uniform
//	std::vector<glm::mat4> currentPose = {};
//	currentPose.resize(boneCount, identity); // use this for no animation
//
//	unsigned int shader = createShader(vertexShaderSource, fragmentShaderSource);
//
//	//get all shader uniform locations
//	unsigned int viewProjectionMatrixLocation = glGetUniformLocation(shader, "view_projection_matrix");
//	unsigned int modelMatrixLocation = glGetUniformLocation(shader, "model_matrix");
//	unsigned int boneMatricesLocation = glGetUniformLocation(shader, "bone_transforms");
//	unsigned int textureLocation = glGetUniformLocation(shader, "diff_texture");
//
//	// initialize projection view and model matrix
//	glm::mat4 projectionMatrix = glm::perspective(70.0f, (float)DisplayManager::WIDTH / DisplayManager::HEIGHT, 0.01f, 1000.0f);
//
//	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.2f, -5.0f)
//		, glm::vec3(0.0f, .0f, 0.0f),
//		glm::vec3(0, 1, 0));
//	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
//
//	glm::mat4 modelMatrix(1.0f);
//	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
//	modelMatrix = glm::scale(modelMatrix, glm::vec3(.2f, .2f, .2f));
//
//
//	//update loop
//	while (isRunning) {
//		SDL_Event ev;
//		while (SDL_PollEvent(&ev)) {
//			if (ev.type == SDL_QUIT)
//				isRunning = false;
//		}
//
//		float elapsedTime = (float)SDL_GetTicks() / 1000;
//
//		float dAngle = elapsedTime * 0.002;
//		modelMatrix = glm::rotate(modelMatrix, dAngle, glm::vec3(0, 1, 0));
//
//		getPose(animation, skeleton, elapsedTime, currentPose, identity, globalInverseTransform);
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glUseProgram(shader);
//		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
//		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
//		glUniformMatrix4fv(boneMatricesLocation, boneCount, GL_FALSE, glm::value_ptr(currentPose[0]));
//
//		glBindVertexArray(vao);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
//		glUniform1i(textureLocation, 0);
//
//		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//
//		SDL_GL_SwapWindow(window);
//	}
//
//	//cleanup
//	SDL_GLContext context = SDL_GL_GetCurrentContext();
//	SDL_GL_DeleteContext(context);
//	SDL_DestroyWindow(window);
//	SDL_Quit();
//}
//
////1. 2.all mesh infomation 3.model struct 4. 5. 6.
//void Mesh::loadModel(const aiScene* scene, aiMesh* mesh, std::vector<Vertex>& verticesOutput, 
//	std::vector<unsigned int>& indicesOutput, Bone& skeletonOutput, unsigned int &nBoneCount)
//{
//	//output list
//	verticesOutput = {};
//	indicesOutput = {};
//
//	//load position, normal, textureCoords
//	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
//	{
//		//process position
//		Vertex vertex;
//		glm::vec3 vector;
//		vector.x = mesh->mVertices[i].x;
//		vector.y = mesh->mVertices[i].y;
//		vector.z = mesh->mVertices[i].z;
//		vertex.positions = vector;
//
//		//process normal
//		vector.x = mesh->mNormals[i].x;
//		vector.y = mesh->mNormals[i].y;
//		vector.z = mesh->mNormals[i].z;
//		vertex.normals = vector;
//
//		//process textureCoords
//		glm::vec2 vec;
//		vec.x = mesh->mTextureCoords[0][i].x;
//		vec.y = mesh->mTextureCoords[0][i].y;
//		vertex.textCoords = vec;
//
//		vertex.boneIds = glm::ivec4(0);
//		vertex.boneWeights = glm::vec4(0.0f);
//
//		//set position, normals, textureCoords to list
//		verticesOutput.push_back(vertex);
//	}
//
//	//-------------------bone------------------------------------------
//	//load boneData to vertices
//	std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo;
//	std::vector<unsigned int>boneCount;
//	boneCount.resize(verticesOutput.size(), 0);
//	nBoneCount = mesh->mNumBones;
//
//	//loop through each bone
//	for (unsigned int i = 0; i < nBoneCount; i++)
//	{
//		aiBone* bone = mesh->mBones[i];
//		glm::mat4 m = assimpToGlmMatrix(bone->mOffsetMatrix);
//		boneInfo[bone->mName.C_Str()] = { i, m };
//
//		//loop throuth each vertex have this bone
//		for (int j = 0; j < bone->mNumWeights; j++)
//		{
//			//weight ID
//			unsigned int id = bone->mWeights[j].mVertexId;
//			//weight
//			float weight = bone->mWeights[j].mWeight;
//			boneCount[id] ++;
//			switch (boneCount[id])
//			{
//			case 1:
//				verticesOutput[id].boneIds.x = i;
//				verticesOutput[id].boneWeights.x = weight;
//				break;
//
//			case 2:
//				verticesOutput[id].boneIds.y = i;
//				verticesOutput[id].boneWeights.y = weight;
//				break;
//
//			case 3:
//				verticesOutput[id].boneIds.z = i;
//				verticesOutput[id].boneWeights.z = weight;
//				break;
//
//			case 4:
//				verticesOutput[id].boneIds.w = i;
//				verticesOutput[id].boneWeights.w = weight;
//				break;
//
//			default:
//				break;
//			}
//		}
//	}
//
//	//normalize weights to make all weights sum
//	for (int i = 0; i < verticesOutput.size(); i++)
//	{
//		glm::vec4 & boneWeights = verticesOutput[i].boneWeights;
//		float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
//
//		if (totalWeight > 0.0f)
//		{
//			verticesOutput[i].boneWeights = glm::vec4
//			(
//				boneWeights.x / totalWeight,
//				boneWeights.y / totalWeight,
//				boneWeights.z / totalWeight,
//				boneWeights.w / totalWeight
//			);
//		}
//	}
//
//	//load indices
//	for (int i = 0; i < mesh->mNumFaces; i++)
//	{
//		aiFace& face = mesh->mFaces[i];
//		for (unsigned int j = 0; j < face.mNumIndices; j++)
//		{
//			indicesOutput.push_back(face.mIndices[j]);
//		}
//	}
//	
//	readSkeleton(skeletonOutput, scene->mRootNode, boneInfo);
//}
//
////a recursive function to read all bone and form skeleton
//bool Mesh::readSkeleton(Bone & boneOutput, aiNode * node, std::unordered_map<std::string, std::pair<int, glm::mat4>>& boneInfoTable)
//{
//	//if node is actually a bone
//	if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
//	{
//		boneOutput.name = node->mName.C_Str();
//		boneOutput.id = boneInfoTable[boneOutput.name].first;
//		boneOutput.offset = boneInfoTable[boneOutput.name].second;
//
//		for (int i = 0; i < node->mNumChildren; i++)
//		{
//			Bone child;
//			readSkeleton(child, node->mChildren[i], boneInfoTable);
//			boneOutput.children.push_back(child);
//		}
//		return true;
//	}
//	//find bones in children
//	else
//	{
//		for (int i = 0; i < node->mNumChildren; i++)
//		{
//			if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable))
//			{
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
//void Mesh::loadAnimation(const aiScene * scene, Animation & animation)
//{
//	//loading first Animation
//	aiAnimation* anim = scene->mAnimations[0];
//
//	if (anim->mTicksPerSecond != 0.0f)
//	{
//		animation.ticksPerSecond = anim->mTicksPerSecond;
//	}
//	else
//	{
//		animation.ticksPerSecond = 1;
//	}
//
//	animation.duration = anim->mDuration * anim->mTicksPerSecond;
//	animation.boneTransforms;
//
//	//load position rotation and scale for each bone
//	//each channel represents each bone
//	for (int i = 0; i < anim->mNumChannels; i++)
//	{
//		aiNodeAnim* channel = anim->mChannels[i];
//		BoneTransformTrack track;
//		for (int j = 0; j < channel->mNumPositionKeys; j++)
//		{
//			track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
//			track.positions.push_back(assimpToClmVec3(channel->mPositionKeys[j].mValue));
//		}
//		for (int j = 0; j < channel->mNumRotationKeys; j++)
//		{
//			track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
//			track.rotations.push_back(assimpToGlmQuat(channel->mRotationKeys[j].mValue));
//		}
//		for (int j = 0; j < channel->mNumScalingKeys; j++)
//		{
//			track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
//			track.scales.push_back(assimpToClmVec3(channel->mScalingKeys[j].mValue));
//		}
//		animation.boneTransforms[channel->mNodeName.C_Str()] = track;
//	}
//}
//
//unsigned int Mesh::createVertexArray(std::vector<Vertex>& vertices, std::vector<unsigned int> indices)
//{
//	unsigned int vao;
//	unsigned int vbo;
//	unsigned int ebo;
//
//	glGenVertexArrays(1, &vao);
//	glGenBuffers(1, &vbo);
//	glGenBuffers(1, &ebo);
//
//	glBindVertexArray(vao);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
//	glDisableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, positions));
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normals));
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, textCoords));
//	glEnableVertexAttribArray(3);
//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneIds));
//	glEnableVertexAttribArray(4);
//	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, boneWeights));
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
//	glBindVertexArray(0);
//	return vao;
//}
//
////
//unsigned int Mesh::createTexture(std::string filepath)
//{
//	unsigned int textureId = 0;
//	int width, height, nrChannels;
//	byte* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 4);
//	glGenTextures(1, &textureId);
//	glBindTexture(GL_TEXTURE_2D, textureId);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//
//	stbi_image_free(data);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	return textureId;
//}
//
//std::pair<unsigned int, float> getTimeFraction(std::vector<float>& times, float& dt) {
//	unsigned int segment = 0;
//	while (dt > times[segment])
//		segment++;
//	float start = times[segment - 1];
//	float end = times[segment];
//	float frac = (dt - start) / (end - start);
//	return{ segment, frac };
//}
//
//void Mesh::getPose(Animation & animation, Bone & skeletion, float dt, std::vector<glm::mat4>& output, glm::mat4 & parentTransform, glm::mat4 & globalInverseTransform)
//{
//	BoneTransformTrack& btt = animation.boneTransforms[skeletion.name];
//	dt = fmod(dt, animation.duration);
//	std::pair<unsigned int, float> fp;
//	//calculate interpolated position
//	fp = getTimeFraction(btt.positionTimestamps, dt);
//
//	glm::vec3 position1 = btt.positions[fp.first - 1];
//	glm::vec3 position2 = btt.positions[fp.first];
//
//	glm::vec3 position = glm::mix(position1, position2, fp.second);
//
//	//calculate interpolated rotation
//	fp = getTimeFraction(btt.rotationTimestamps, dt);
//	glm::quat rotation1 = btt.rotations[fp.first - 1];
//	glm::quat rotation2 = btt.rotations[fp.first];
//
//	glm::quat rotation = glm::slerp(rotation1, rotation2, fp.second);
//
//	//calculate interpolated scale
//	fp = getTimeFraction(btt.scaleTimestamps, dt);
//	glm::vec3 scale1 = btt.scales[fp.first - 1];
//	glm::vec3 scale2 = btt.scales[fp.first];
//
//	glm::vec3 scale = glm::mix(scale1, scale2, fp.second);
//
//	glm::mat4 positionMat = glm::mat4(1.0),
//		scaleMat = glm::mat4(1.0);
//
//
//	// calculate localTransform
//	positionMat = glm::translate(positionMat, position);
//	glm::mat4 rotationMat = glm::toMat4(rotation);
//	scaleMat = glm::scale(scaleMat, scale);
//	glm::mat4 localTransform = positionMat * rotationMat * scaleMat;
//	glm::mat4 globalTransform = parentTransform * localTransform;
//
//	output[skeletion.id] = globalInverseTransform * globalTransform * skeletion.offset;
//	//update values for children bones
//	for (Bone& child : skeletion.children) {
//		getPose(animation, child, dt, output, globalTransform, globalInverseTransform);
//	}
//}
//
//
