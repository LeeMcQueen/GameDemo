#include "TexturedModel.h"

#include "RawModel.h"
#include "ModelTexture.h"

TexturedModel::TexturedModel(const RawModel &rawmodel, const ModelTexture &modelTexture)
	:rawModel_(rawmodel)
	, modelTexture_(modelTexture)
{}

TexturedModel::~TexturedModel()
{

}

//get RawModel (vao ID , vertex number)
RawModel TexturedModel::GetRawModel() const
{
	return rawModel_;
}

//get modelTexture (textureID, shineDamper, reflectivity)
ModelTexture TexturedModel::GetTextureModel() const
{
	return	modelTexture_;
}
