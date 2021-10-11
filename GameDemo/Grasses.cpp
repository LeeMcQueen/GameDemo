
#include "grasses.h"
#include "std_image.h"

#include <random>
#include <vector>
#include <glm.hpp>
#include <GLFW/glfw3.h>

// Indirect drawing structure
struct NumBlades {
	std::uint32_t vertexCount = 5;
	std::uint32_t instanceCount = 1;
	std::uint32_t firstVertex = 0;
	std::uint32_t firstInstance = 0;
};

struct Blade {

	Blade(glm::vec4 p_v0, glm::vec4 p_v1, glm::vec4 p_v2, glm::vec4 p_up) :
		v0(p_v0),
		v1(p_v1),
		v2(p_v2),
		up(p_up){};

	glm::vec4 v0; // xyz: Position, w: 半径上的方向 orientation (in radius)
	glm::vec4 v1; // xyz: 贝塞尔点 Bezier point w: 草的高
	glm::vec4 v2; // xyz: Physical model guide w: 草的宽
	glm::vec4 up; // xyz: Up vector w: stiffness coefficient草的回复力
};

namespace {

	std::vector<Blade> generate_blades()
	{

		int _width, _height, _colorChannels;
		unsigned char *_image;
		_image = stbi_load("res/heightmap3.png", &_width, &_height, &_colorChannels, 0);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> orientation_dis(0, 3.1415926);
		std::uniform_real_distribution<float> height_dis(4.0f, 6.0f);
		std::uniform_real_distribution<float> dis(0, 1);

		std::vector<Blade> blades;
		for (int i = 0; i < 256; ++i) {
			for (int j = 0; j < 256; ++j) {
				const auto x = static_cast<float>(j) + dis(gen) * 0.1f;		//草的位置X
				const auto y = static_cast<float>(i) + dis(gen) * 0.1f;		//草的位置Y
				const auto blade_height = height_dis(gen);

				int addr = (i * _width + j) * _colorChannels;
				std::int32_t r = _image[addr];
				std::int32_t g = _image[addr + 1];
				std::int32_t b = _image[addr + 2];
				float height1 = (r << 16) + (g << 8) + b;
				height1 /= (256 * 256 * 256 / 2);
				height1 -= 1.0;
				height1 *= 40;

				blades.emplace_back(
					glm::vec4(x, height1, y, orientation_dis(gen)),
					glm::vec4(x, blade_height, y, blade_height),
					glm::vec4(x, blade_height, y, 1.0f),
					glm::vec4(0, blade_height, 0, 0.7f + dis(gen) * 0.3f));
			}
		}
		return blades;
	}

}

void Grasses::init(){

	const std::vector<Blade> blades = generate_blades();
	blades_count_ = static_cast<GLuint>(blades.size());

	glPatchParameteri(GL_PATCH_VERTICES, 1);

	glGenVertexArrays(1, &grass_vao_);
	glBindVertexArray(grass_vao_);

	unsigned int grass_input_buffer = 0;
	glGenBuffers(1, &grass_input_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, grass_input_buffer);
	//分配数据
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		static_cast<GLsizei>(blades.size() * sizeof(Blade)),
		blades.data(), GL_DYNAMIC_COPY);
	//layout的binding号对应
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grass_input_buffer);

	unsigned int grass_output_buffer = 0;
	glGenBuffers(1, &grass_output_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, grass_output_buffer);
	//分配数据
	glBufferData(GL_SHADER_STORAGE_BUFFER,
		static_cast<GLsizei>(blades.size() * sizeof(Blade)), nullptr,
		GL_STREAM_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, grass_output_buffer);

	NumBlades numBlades;
	unsigned int grass_indirect_buffer = 0;
	glGenBuffers(1, &grass_indirect_buffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, grass_indirect_buffer);
	//分配数据
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(NumBlades), &numBlades,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, grass_output_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grass_indirect_buffer);

	// v0 attribute
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
		reinterpret_cast<void*>(offsetof(Blade, v0)));
	glEnableVertexAttribArray(0);

	// v1 attribute
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
		reinterpret_cast<void*>(offsetof(Blade, v1)));
	glEnableVertexAttribArray(1);

	// v2 attribute
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
		reinterpret_cast<void*>(offsetof(Blade, v2)));
	glEnableVertexAttribArray(2);

	// dir attribute
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Blade),
		reinterpret_cast<void*>(offsetof(Blade, up)));
	glEnableVertexAttribArray(3);

	grass_compute_shader_ =
		ShaderBuilder{}.load("Shader/grass.comp.glsl", Shader::Type::Compute).build();
	grass_compute_shader_.use();

	grass_shader_ = ShaderBuilder{}
		.load("Shader/grass.vert.glsl", Shader::Type::Vertex)
		.load("Shader/grass.tesc.glsl", Shader::Type::TessControl)
		.load("Shader/grass.tese.glsl", Shader::Type::TessEval)
		.load("Shader/grass.frag.glsl", Shader::Type::Fragment)
		.build();
}

void Grasses::update(DeltaDuration delta_time){

	grass_compute_shader_.use();
	grass_compute_shader_.setFloat("current_time",
		static_cast<float>(glfwGetTime()));
	grass_compute_shader_.setFloat("delta_time", delta_time.count() / 1e3f);
	grass_compute_shader_.setFloat("wind_magnitude", wind_magnitude);
	grass_compute_shader_.setFloat("wind_wave_length", wind_wave_length);
	grass_compute_shader_.setFloat("wind_wave_period", wind_wave_period);

	//gl_NumWorkGrounps 执行16000次WorkGrounps
	glDispatchCompute(static_cast<GLuint>(blades_count_), 1, 1);
}

void Grasses::render(){

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindVertexArray(grass_vao_);
	grass_shader_.use();
	glDrawArraysIndirect(GL_PATCHES, reinterpret_cast<void*>(0));
}

//计算草在当前位置的高度
float Grasses::getGrassesHeight(int x, int z, unsigned char * image){


	return 0.0f;
}

std::int32_t Grasses::getRGBSum(int x, int y){

	return std::int32_t();
}
