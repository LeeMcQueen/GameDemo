#include "RawModel.h"

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
