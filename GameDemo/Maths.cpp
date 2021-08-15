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
