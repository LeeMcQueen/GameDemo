#include <cmath>

#include "Maths.h"

//OBJ的移动控制函数 (位置，旋转，缩放)
glm::mat4 Maths::createTransformationMatrix(const glm::vec3 position, const glm::vec3 rotation, const glm::vec3 scale)
{
	glm::mat4 modelMatrix;

	//计算位移矩阵
	modelMatrix = glm::translate(modelMatrix, position);

	//旋转矩阵根据X，Y，Z轴
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	//缩放矩阵
	modelMatrix = glm::scale(modelMatrix, glm::vec3{ scale.x, scale.y, scale.x });

	//把位移，旋转，缩放合并成modelMatrix然后传给vertexShader
	return modelMatrix;
}

glm::mat4 Maths::createTransformationMatrix(glm::vec2 & translation, glm::vec2 & scale){

	glm::mat4 modelMatrix;

	modelMatrix = glm::translate(modelMatrix, glm::vec3{ translation.x, translation.y, 0.0f });
	modelMatrix = glm::scale(modelMatrix, glm::vec3{ scale.x, scale.y, 0.0f });

	return modelMatrix;
}

float Maths::barryCentricInterpolation(const glm::vec3 & p1, const glm::vec3 & p2, const glm::vec3 & p3, const glm::vec2 & pos){

	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}
