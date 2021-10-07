#pragma once

#include "GrassesShader.h"

#include <chrono>

class Grasses {
	unsigned int grass_vao_ = 0;
	ShaderPrograms grass_shader_{};
	ShaderPrograms grass_compute_shader_{};
	GLuint blades_count_ = 0;

public:
	// Wind parameters
	float wind_magnitude = 1.0;
	float wind_wave_length = 1.0;
	float wind_wave_period = 1.0;

	using DeltaDuration = std::chrono::duration<float, std::milli>;

	void init();
	void update(DeltaDuration delta_time);
	void render();

private:

	float getGrassesHeight(int x, int z, unsigned char *image);
	std::int32_t getRGBSum(int x, int y);
};
