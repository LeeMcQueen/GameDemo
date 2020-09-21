#pragma once

#include "RawModel.h"
#include "ModelTexture.h"

class TexturedModel
{
public:
	TexturedModel(RawModel& model, ModelTexture& texture);
	~TexturedModel();

	RawModel GetRawModel() const;
	ModelTexture GetTextureModel() const;

private:

	RawModel rawModel_;

	ModelTexture modelTexture_;
};

