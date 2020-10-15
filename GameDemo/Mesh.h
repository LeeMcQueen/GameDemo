#pragma once

#include <glm.hpp>
#include <string>

struct Vertex {
	glm::vec3 vertices;
	glm::vec2 texCoords;
	glm::vec3 normals;
};

struct Vertex {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh();

private:

};

