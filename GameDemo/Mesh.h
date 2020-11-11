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

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textCoords;
};

struct Texture
{
	GLuint id;
	string type;
	aiString path;
};

struct BoneMartrix
{
	aiMatrix4x4 offset_matrix;
	aiMatrix4x4 final_world_transform;
};

struct VertexBoneData
{
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

class Mesh
{
public:

	Mesh(vector<Vertex> vertex, vector<GLuint> ind, vector<Texture> texture, vector<VertexBoneData> bonr_id_weights);
};