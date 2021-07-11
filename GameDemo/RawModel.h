#pragma once
#include <GL/glew.h>

class RawModel{

public:
	//Constructor
	RawModel(GLuint vaoId, GLuint vertexCount);

	//get VAO id
	GLuint getVaoId() const;

	//get vertex number
	GLuint getVertexCount() const;

private:
	GLuint vaoId_;
	GLuint vertexCount_;
	GLuint* vboId_;
	int vboCount_;

};

