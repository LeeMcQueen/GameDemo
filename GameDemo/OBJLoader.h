#pragma once

#include <glm.hpp>
#include <gtx/string_cast.hpp>

#include "RawModel.h"
#include "Loader.h"

using namespace std;

class OBJLoader
{
public:

	//load an obj file
	static RawModel loadModel(const char* fileName);

	//faster OBJLoader function
	static RawModel tinyOBJLoader(const char* fileName);

private:

	static void processVertex(const vector<string>& vertexData, vector<glm::vec2>& textures, 
		vector<glm::vec3>& normals);

	//Function split
	static std::vector<string> split(const string &phrase, const string&delimiter);
	//Function startWith
	static bool startsWith(const string& line, const string& pref);

	//vertex position data
	static vector<float> verticesArray;
	//texture poistion data
	static vector<float> texturesArray;
	//normal position data
	static vector<float> normalsArray;
	//face data
	static vector<int> indices;
};

