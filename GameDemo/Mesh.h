#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "glew.h"
#include "glm.hpp"
#include "assimp\Importer.hpp"

#define NUM_BONES_PER_VEREX 4
using namespace std;
typedef unsigned int uint;

struct Vertex{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textCoords;
};

struct Texture{
	GLuint id;
	string type;
	aiString path;
};

struct BoneMartrix{
	aiMatrix4x4 offset_matrix;
	aiMatrix4x4 final_world_transform;
};

struct VertexBoneData{
	//4 bone id for each vertex
	uint ids[NUM_BONES_PER_VEREX];
	//4 weights for each vertex
	float weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		//init all value in array(0)
		memset(ids, 0, sizeof(ids));
		memset(weights, 0, sizeof(weights));
	}

	void addBoneData(uint bone_id, float weight);
};

class Mesh{
public:

	Mesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> textures, vector<int> indices);
	~Mesh();

	void Draw(GLuint shadersProgram);

private:

	vector<glm::vec3> vertices_;
	vector<glm::vec3> normals_;
	vector<glm::vec2> textures_;
	vector<int> indices_;

	//GLuint VAO;
	GLuint VBO_vertices;
	GLuint VBO_bones;
	GLuint EBO_indices;

	void SetupMesh();
};