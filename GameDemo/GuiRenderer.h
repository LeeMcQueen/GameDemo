#pragma once

#include "RawModel.h"
#include "Loader.h"
#include "GuiTexture.h"
#include "GuiShader.h"
#include "Maths.h"

class GuiRenderer {

public:

	GuiRenderer(Loader loader);
	static void render(std::vector<GuiTexture> guiTextures);
	static void cleanUp();

private:

	static RawModel rawModel_;
	static GuiShader guiShader_;
};