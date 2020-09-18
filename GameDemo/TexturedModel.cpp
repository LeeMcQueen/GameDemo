#include "TexturedModel.h"

#include "RawModel.h"
#include "ModelTexture.h"

TexturedModel::TexturedModel(RawModel& model, ModelTexture& texture)
	:rawModel_(model)
	, modelTexture_(texture)
{}

TexturedModel::~TexturedModel()
{

}

RawModel TexturedModel::GetRawModel() const
{
	return rawModel_;
}

ModelTexture TexturedModel::GetTextureModel() const
{
	return	modelTexture_;
}
