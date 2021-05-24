#include "Mesh.h"



Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
	:vertices_(vertices)
	,indices_(indices)
	,textures_(textures){

	//初始化
	SetupMesh();
}

Mesh::~Mesh(){
}

void VertexBoneData::addBoneData(uint bone_id, float weight){
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

void Mesh::Draw(GLuint shadersProgram){
	//漫反射贴图
	int diffuse_nr = 1;
	//高光反射贴图
	int specular_nr = 1;
	
	//贴图功能
	for (int i = 0; i < vertices_.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		string number;
		string name = textures_[i].type;

		//如果type是漫反射贴图
		if (name == "texture_diffues")
		{
			number = to_string(diffuse_nr++);
		}
		else if (name == "texture_specular")
		{
			number = to_string(specular_nr++);
		}

		glBindTexture(GL_TEXTURE_2D, textures_[i].id);
		glUniform1i(glGetUniformLocation(shadersProgram, ("material." + name + number).c_str()), i);

	}
}

void Mesh::SetupMesh() {

	//顶点（vertices）坐标date
	glGenBuffers(1, &VBO_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(vertices_[0]), &vertices_[0], GL_STATIC_DRAW);
	//解绑
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//骨骼(bones) data
	glGenBuffers(1, &VBO_bones);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
	glBufferData(GL_ARRAY_BUFFER, bones_id_weight_for_each_vertex_.size() * sizeof(bones_id_weight_for_each_vertex_[0]), &bones_id_weight_for_each_vertex_[0], GL_STATIC_DRAW);
	//解绑
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//EBO
	glGenBuffers(1, &EBO_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), &indices_[0], GL_STATIC_DRAW);
	//EBO解绑
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//生成VAO 从buffers得到data给shaders
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);

	//顶点位置 vertex position
	//glEnableVertexAttribArray();

}

