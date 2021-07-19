#include "RawModel.h"

#define GLEW_STATIC
//#include <GL/glew.h>
#include <GLFW/glfw3.h>

RawModel::RawModel(GLuint vaoId, GLuint vertexCount)
	: vaoId_(vaoId)
	, vertexCount_(vertexCount)
{ }

GLuint RawModel::getVaoId() const
{
	return vaoId_;
}

GLuint RawModel::getVertexCount() const
{
	return vertexCount_;
}
