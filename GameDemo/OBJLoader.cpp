#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <stdio.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "OBJLoader.h"

using namespace std;

//data array
vector<float> OBJLoader::verticesArray;
vector<float> OBJLoader::texturesArray;
vector<float> OBJLoader::normalsArray;
vector<int> OBJLoader::indices;

RawModel OBJLoader::loadObjModel(const std::string & fileName)
{
	clock_t startTime = clock();
	// Open the file as read only
	FILE* file;
	if (fopen_s(&file, ("res/" + fileName + ".obj").c_str(), "r") != 0)
	{
		printf("Failed to open: %s\n", fileName);
	}

	// Storage variables
	std::vector<glm::vec2> textures, tempTextures;
	std::vector<glm::vec3> vertices, normals, tempNormals;
	std::vector<int> indices;

	char *type, *token, *stop = 0;
	double x, y, z;
	char line[256];
	while (fgets(line, 256, file) != NULL)
	{
		token = NULL;
		type = strtok_s(line, " ", &token);
		// V is vertex points
		if (type[0] == 'v' && type[1] == NULL)
		{
			x = strtod(token, &stop);
			token = stop + 1; // Move to the next value
			y = strtod(token, &stop);
			token = stop + 1; // Move to the next value
			z = strtod(token, &stop);
			// Store a new vertex
			vertices.push_back(glm::vec3(x, y, z));
		}
		// VT is vertex texture coordinates
		else if (type[0] == 'v' && type[1] == 't')
		{
			x = strtod(token, &stop);
			token = stop + 1; // Move to the next value
			y = 1 - strtod(token, &stop);
			// Store a new texture
			tempTextures.push_back(glm::vec2(x, y));
		}
		else if (type[0] == 'v' && type[1] == 'n')
		{
			x = strtod(token, &stop);
			token = stop + 1; // Move to the next value
			y = strtod(token, &stop);
			token = stop + 1; // Move to the next value
			z = strtod(token, &stop);
			// Store a new normal
			tempNormals.push_back(glm::vec3(x, y, z));
		}
		// F is the index list for faces
		else if (type[0] == 'f')
		{
			if (indices.size() == 0)
			{
				// Set the size of the array
				textures.resize(vertices.size());
				normals.resize(vertices.size());
			}
			// Process set of vertex data
			ProcessVertices(token, indices, tempTextures, textures, tempNormals, normals);
		}
	}
	fclose(file);

	printf("Load time: %dms\n", clock() - startTime);

	Loader* myloader = new Loader;
	return myloader->loadToVao(vertices, textures, normals, indices);
}

void OBJLoader::ProcessVertices(char * vertexData,
	std::vector<int>& indices,
	std::vector<glm::vec2>& tempTextures,
	std::vector<glm::vec2>& textures,
	std::vector<glm::vec3>& tempNormals,
	std::vector<glm::vec3>& normals){
	char *stop;
	int vertexPointer;
	for (unsigned int i = 0; i < 3; i++)
	{
		// Get and store index
		vertexPointer = strtol(vertexData, &stop, 10) - 1;
		indices.push_back(vertexPointer);
		vertexData = stop + 1; // Move to the next value
							   // Get and store texture points
		textures[vertexPointer] = tempTextures[strtol(vertexData, &stop, 10) - 1];
		vertexData = stop + 1; // Move to the next value
							   // Get and store normal points
		normals[vertexPointer] = tempNormals[strtol(vertexData, &stop, 10) - 1];
		vertexData = stop + 1; // Move to the next value
	}
}

//tiny_obj_loader
RawModel OBJLoader::tinyOBJLoader(const std::string& fileName)
{
	string path = "res/" + string(fileName) + ".obj";

	vector<int> indices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
		throw std::runtime_error(err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			//positions
			positions.emplace_back(
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			);

			//texture coordinates
			texCoords.emplace_back(
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.f - attrib.texcoords[2 * index.texcoord_index + 1]
			);

			//normals
			normals.emplace_back(
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			);

			indices.push_back(static_cast<unsigned int &&>(indices.size()));
		}
	}

	Loader* myloader = new Loader;
	return myloader->loadToVao(positions, texCoords, normals, indices);
}

RawModel OBJLoader::loadModel(const std::string& fileName)
{
	//initialization all data
	verticesArray.clear();
	texturesArray.clear();
	normalsArray.clear();
	indices.clear();

	//set path
	auto path = "res/" + string(fileName) + ".obj";
	ifstream objData(path);
	if (!objData.is_open())
	{
		cerr << "ERROR: [OBJLoader::loadOBJ] Cannot load OBJModel" << fileName << "!" << endl;
	}

	//data vectex & texture & normal
	vector<glm::vec3> vertices;
	vector<glm::vec2> textures;
	vector<glm::vec3> normals;

	//temp data
	string line;
	while (getline(objData, line))
	{
		vector<string> currentLine = split(line, " ");

		//OBJFile begin V => vertex
		if (startsWith(line, "v "))
		{
			glm::vec3 vertex(stof(currentLine[1]), stof(currentLine[2]), stof(currentLine[3]));
			vertices.emplace_back(vertex);
		}
		//OBJFile begin vt => texture
		else if (startsWith(line, "vt "))
		{
			glm::vec2 texture(stof(currentLine[1]), stof(currentLine[2]));
			textures.emplace_back(texture);
		}
		//OBJFile begin vn => noraml
		else if (startsWith(line, "vn"))
		{
			glm::vec3 normal(stof(currentLine[1]), stof(currentLine[2]), stof(currentLine[3]));
			normals.emplace_back(normal);
		}
		//OBJFile begin f => face
		else if (startsWith(line, "f "))
		{
			texturesArray.resize(vertices.size() * 2);
			normalsArray.resize(vertices.size() * 3);
			break;
		}

	}

	while (!line.empty())
	{
		if (!startsWith(line, "f"))
		{
			getline(objData, line);
			continue;
		}

		auto current_line = split(line, " ");
		vector<string> vertex1 = split(current_line.at(1), "/");
		vector<string> vertex2 = split(current_line.at(2), "/");
		vector<string> vertex3 = split(current_line.at(3), "/");

		processVertex(vertex1, textures, normals);
		processVertex(vertex2, textures, normals);
		processVertex(vertex3, textures, normals);

		getline(objData, line);
	}

	objData.close();

	verticesArray.reserve(vertices.size() * 3);
	for (const auto& vertex : vertices)
	{
		verticesArray.push_back(vertex.x);
		verticesArray.push_back(vertex.y);
		verticesArray.push_back(vertex.z);
	}

	Loader* myloader = new Loader;
	return myloader->loadToVAO(verticesArray, texturesArray, normalsArray, indices);
}

void OBJLoader::processVertex(const vector<string>& vertexData, vector<glm::vec2>& textures, vector<glm::vec3>& normals)
{
	int current_vertex_pointer = stoi(vertexData.at(0)) - 1;
	indices.emplace_back(current_vertex_pointer);

	auto currentTexture = textures.at(std::stoi(vertexData[1]) - 1);
	texturesArray.at(current_vertex_pointer * 2) = currentTexture.x;
	texturesArray.at(current_vertex_pointer * 2 + 1) = 1 - currentTexture.y;

	auto currentNormal = normals.at(std::stoi(vertexData[2]) - 1);
	normalsArray.at(current_vertex_pointer * 3) = currentNormal.x;
	normalsArray.at(current_vertex_pointer * 3 + 1) = currentNormal.y;
	normalsArray.at(current_vertex_pointer * 3 + 2) = currentNormal.z;
}



//function split
std::vector<string> OBJLoader::split(const string & phrase, const string & delimiter)
{
	vector<string> list;

	string temp = string(phrase);

	size_t pos = 0;

	string token;

	while ((pos = temp.find(delimiter)) != string::npos)
	{
		token = temp.substr(0, pos);
		list.push_back(token);
		temp.erase(0, pos + delimiter.length());
	}
	list.push_back(temp);

	return list;
}

//function startsWith
bool OBJLoader::startsWith(const string & line, const string & pref)
{
	return equal(pref.begin(), pref.end(), line.begin());
}
