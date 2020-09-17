#pragma once

#include "RawModel.h"
#include "TexturedModel.h"

class Renderer
{
public:
	void Prepare();

	void Render(TexturedModel texturedModel);
};

