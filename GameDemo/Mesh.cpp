#include "Mesh.h"
#include "Loader.h"


Mesh::Mesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> textures, vector<int> indices)
	:vertices_(vertices)	
	,textures_(textures)
	, normals_(normals)
	, indices_(indices) {

	//初始化
	SetupMesh();
}

Mesh::~Mesh(){
}

void Mesh::Draw(GLuint shadersProgram){

}

void Mesh::SetupMesh() {

	Loader loader;
	loader.loadToVao(vertices_, textures_, normals_, indices_);

}





void VertexBoneData::addBoneData(uint bone_id, float weight) {
	for (uint i = 0; i < NUM_BONES_PER_VEREX; i++)
	{
		if (weights[i] == 0.0)
		{
			ids[i] = bone_id;
			weights[i] = weight;
			return;
		}
	}
}

