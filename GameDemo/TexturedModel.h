#pragma once

#include "RawModel.h"
#include "ModelTexture.h"

class TexturedModel
{
public:
	explicit TexturedModel(const RawModel& rawmodel, const ModelTexture& modelTexture);
	~TexturedModel();

	RawModel GetRawModel() const;
	ModelTexture GetTextureModel() const;

	bool operator <(const TexturedModel &rhs) const
	{
		return (modelTexture_.getID() < rhs.GetTextureModel().getID());
	}

private:

	RawModel rawModel_;

	ModelTexture modelTexture_;
};

