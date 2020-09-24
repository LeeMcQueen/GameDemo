#include <cmath>

#include "Maths.h"

glm::mat4 Maths::createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	//calculate translation Matrix
	glm::mat4 transformationMatrix = glm::translate(position);

	//calculate Rotation Matrix
	glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1, 0, 0))*
		glm::rotate(rotation.y, glm::vec3(0, 1, 0))*
		glm::rotate(rotation.z, glm::vec3(0, 0, 1));

	//calculate Scaling Matrix
	glm::mat4 scaleMatrix = glm::scale(scale);

	return transformationMatrix * rotationMatrix * scaleMatrix;

	////initialize a 4x4 identity matrix
	//float* res = new float[4 * 4];
	//for (int i = 0; i < 4; i++)
	//	for (int j = 0; j < 4; j++)
	//		res[i * 4 + j] = (i == j);

	////calculate Scaling Matrix
	//float* scalingMatrix = new float[4 * 4]{
	//	scale, 0, 0, 0,
	//	0, scale, 0, 0,
	//	0, 0, scale, 0,
	//	0, 0, 0, 1
	//};
	//leftMultiMatrix(res, scalingMatrix);
	//delete[] scalingMatrix;

	//calculateTransformationMatrix(res, position);
	//calculateRotationMatrix(res, rotation);

	//return res;
}

glm::mat4 Maths::createViewMatrix(Camera & camera)
{
	glm::mat4 view;

	view = glm::rotate(view, glm::radians(camera.getRotation().x), { 1, 0, 0 });
	view = glm::rotate(view, glm::radians(camera.getRotation().y), { 0, 1, 0 });
	view = glm::translate(view, -camera.getPosition());

	return view;
}

////Calculate A = B*A, where A and B are both 4x4 Matrix 
//void Maths::leftMultiMatrix(float * A, float * B)
//{
//	float res[16] = { 0 };
//	for (int i = 0; i < 4; i++)
//		for (int j = 0; j < 4; j++)
//			for (int k = 0; k < 4; k++)
//				res[j * 4 + i] += B[k * 4 + i] * A[j * 4 + k];
//	for (int i = 0; i < 16; i++)
//		A[i] = res[i];
//}
//
////calculate Rotation Matrix
//void Maths::calculateRotationMatrix(float * res, vector3 rot, const char * order)
//{
//	for (int i = 0; i < 3; i++)
//	{
//		if (order[i] == 'x')
//		{
//			// Pitch
//			float* rotateXMatrix = new float[4 * 4]{
//				1, 0, 0, 0,
//				0, (float)cos(rot.x), (float)sin(rot.x), 0,
//				0, (float)-sin(rot.x), (float)cos(rot.x), 0,
//				0, 0, 0, 1
//			};
//			leftMultiMatrix(res, rotateXMatrix);
//			delete[] rotateXMatrix;
//		}
//		else if (order[i] == 'y')
//		{
//			// Yaw
//			float* rotateYMatrix = new float[4 * 4]{
//				(float)cos(rot.y), 0, (float)-sin(rot.y), 0,
//				0, 1, 0, 0,
//				(float)sin(rot.y), 0, (float)cos(rot.y), 0,
//				0, 0, 0, 1
//			};
//			leftMultiMatrix(res, rotateYMatrix);
//			delete[] rotateYMatrix;
//		}
//		else
//		{
//			// Roll
//			float* rotateZMatrix = new float[4 * 4]{
//				(float)cos(rot.z), (float)sin(rot.z), 0, 0,
//				(float)-sin(rot.z), (float)cos(rot.z), 0, 0,
//				0, 0, 1, 0,
//				0, 0, 0, 1
//			};
//			leftMultiMatrix(res, rotateZMatrix);
//			delete[] rotateZMatrix;
//		}
//	}
//}
//
////calculate translation Matrix
//void Maths::calculateTransformationMatrix(float * res, vector3 position)
//{
//	float* translationMatrix = new float[4 * 4]
//	{
//		1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		position.x, position.y, position.z, 1
//		//1, 0, 0, position.x,
//		//0, 1, 0, position.y,
//		//0, 0, 1, position.z,
//		//0, 0, 0, 1
//	};
//	leftMultiMatrix(res, translationMatrix);
//	delete[] translationMatrix;
//}
