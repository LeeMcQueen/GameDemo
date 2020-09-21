#pragma once

#include "RawModel.h"
#include "TexturedModel.h"
#include "Entity.h"
#include "StaticShader.h"


class Renderer
{
public:
	void Prepare();

	void Render(Entity entity, StaticShader* shader);
};

