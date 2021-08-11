#include "TerrainShader.h"

//vertexShader&fragmentShader file name
const char* TerrainShader::VERTEX_FILE = "terrainVertexShader.txt";
const char* TerrainShader::FRAGMENT_FILE = "terrainFragmentShader.txt";

TerrainShader::TerrainShader():ShaderProgram(VERTEX_FILE, FRAGMENT_FILE)
{
	//bindAttribute
	bindAttributes();

	//Bind VAO attributes and link program
	glLinkProgram(programID);
	glValidateProgram(programID);

	//add uniform into vertexShader
	getAllUniformLocations();
}

void TerrainShader::bindAttributes()
{
	bindAttribute(0, "position");
	bindAttribute(1, "textureCoords");
	bindAttribute(2, "normal");
}

void TerrainShader::loadTransformationMatrix(glm::mat4 matrix)
{
	loadMatrix4(Location_tansformationMatrix, matrix);
}

void TerrainShader::loadProjectionMatrix(glm::mat4 matrix)
{
	loadMatrix4(Location_projectionMatrix, matrix);
}

void TerrainShader::loadconnectTextureUnits()
{
	//通道0的背景纹理
	loadInt(Location_backgroundTexture, 0);
	//通道1的r纹理
	loadInt(Location_rTexture, 1);
	//通道2的g纹理
	loadInt(Location_gTexture, 2);
	//通道3的b纹理
	loadInt(Location_bTexture, 3);
	//通道4的混合纹理
	loadInt(Location_blendMap, 4);
}

void TerrainShader::loadViewMatrix(glm::mat4 matrix)
{
	loadMatrix4(Location_viewMatrix, matrix);
}

void TerrainShader::loadLight(Light light)
{
	loadVector3(Location_lightColor, light.getColor());
	loadVector3(Location_lightPosition, light.getPosition());
}

void TerrainShader::loadShineVariables(float shineDamper, float reflectivity)
{
	loadFloat(Location_shineDamper, shineDamper);
	loadFloat(Location_reflectivity, reflectivity);
}

void TerrainShader::getAllUniformLocations()
{
	Location_tansformationMatrix = getUniformLocation("transformationMatrix");
	Location_projectionMatrix = getUniformLocation("projectionMatrix");
	Location_viewMatrix = getUniformLocation("viewMatrix");
	Location_lightColor = getUniformLocation("lightColor");
	Location_lightPosition = getUniformLocation("lightPosition");
	Location_shineDamper = getUniformLocation("shineDamper");
	Location_reflectivity = getUniformLocation("reflectivity");
	Location_backgroundTexture = getUniformLocation("backgroundTexture");
	Location_rTexture = getUniformLocation("rTexture");
	Location_gTexture = getUniformLocation("gTexture");
	Location_bTexture = getUniformLocation("bTexture");
	Location_blendMap = getUniformLocation("blendMap");
}
