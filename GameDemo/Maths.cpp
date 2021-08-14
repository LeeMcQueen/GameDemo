#include <cmath>

#include "Maths.h"

//OBJ的移动控制函数 (位置，旋转，缩放)
glm::mat4 Maths::createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	//计算位移矩阵
	glm::mat4 transformationMatrix = glm::translate(position);

	//旋转矩阵根据X，Y，Z轴
	glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1, 0, 0))*
		glm::rotate(rotation.y, glm::vec3(0, 1, 0))*
		glm::rotate(rotation.z, glm::vec3(0, 0, 1));

	//缩放矩阵
	glm::mat4 scaleMatrix = glm::scale(scale);

	//把位移，旋转，缩放合并成modelMatrix然后传给vertexShader
	return transformationMatrix * rotationMatrix * scaleMatrix;
}
