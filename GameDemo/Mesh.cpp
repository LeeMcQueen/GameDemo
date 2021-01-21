#include "Mesh.h"



Mesh::Mesh(vector<Vertex> vertex, vector<GLuint> ind, vector<Texture> texture, vector<VertexBoneData> bonr_id_weights)
{
	vertices = vertex;
	indices = ind;
	texture = texture;
	bones_id_weight_for_each_vertex = bonr_id_weights;

	SetupMesh();
}

Mesh::~Mesh()
{
}

void VertexBoneData::addBoneData(uint bone_id, float weight)
{
	for (uint i = 0; i < NUM_BONES_PER_VEREX; i ++)
	{
		if (weights[i] == 0.0)
		{
			ids[i] = bone_id;
			weights[i] = weight;
			return;
		}
	}
}

void Mesh::Draw(GLuint shaders_program)
{
	//漫反射贴图
	int diffuse_nr = 1;
	//高光反射贴图
	int specular_nr = 1;
	
	//贴图功能
	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		string number;
		string name = textures[i].type;

		//如果type是漫反射贴图
		if (name == "texture_diffues")
		{
			number = to_string(diffuse_nr++);
		}
		else if (name == "texture_specular")
		{
			number = to_string(specular_nr++);
		}

		glBindTexture(GL_TEXTURE_2D,textures[i].id);
		glUniform1i(glGetUniformLocation(shaders_program, ("material." + name + number).c_str()), i);

	}
}

void Mesh::SetupMesh() {

	//顶点（vertices）坐标date
	glGenBuffers(1, &VBO_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);
	//解绑
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//骨骼(bones) date
	glGenBuffers(1, &VBO_bones);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
	glBufferData(GL_ARRAY_BUFFER, bones_id_weight_for_each_vertex.size() * sizeof(bones_id_weight_for_each_vertex[0]), &bones_id_weight_for_each_vertex[0], GL_STATIC_DRAW);
	//解绑
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

