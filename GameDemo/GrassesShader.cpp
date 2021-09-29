#include "GrassesShader.h"

Shader::Shader(const char *source, Type type)
	: type_{ type }, id_{ glCreateShader(std::underlying_type_t<Type>(type)) }{

	glShaderSource(id_, 1, &source, nullptr);
	glCompileShader(id_);
}

Shader::~Shader(){

	glDeleteShader(id_);
}

Shader::Shader(Shader &&other) noexcept : type_{ other.type_ }, id_{ other.id_ }{

	other.id_ = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept{

	std::swap(id_, other.id_);
	std::swap(type_, other.type_);
	return *this;
}

ShaderPrograms::ShaderPrograms(const std::vector<Shader> &shaders)
	: id_{ glCreateProgram() } {

	for (const auto& shader : shaders) {
		glAttachShader(id_, shader.id_);
	}

	glLinkProgram(id_);
}
