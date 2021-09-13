#pragma once

#include "RawModel.h"
#include "Loader.h"
#include "GuiTexture.h"
#include "GuiShader.h"
#include "Maths.h"

class GuiRenderer {

public:

	GuiRenderer(GuiShader &guiShader, Loader loader);
	void render(std::vector<GuiTexture> guiTextures);
	void cleanUp();

private:

	GuiShader guiShader_;
	RawModel rawModel_;
};