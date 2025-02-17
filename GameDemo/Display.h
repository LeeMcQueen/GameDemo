﻿#pragma once

#include <iostream>
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <unordered_map>

#include "Cloth.h"
#include "Rigid.h"
#include "Program.h"
#include "std_image.h"
#include "camera.h"
#include "Loader.h"
#include "masterRenderer.h"
#include "DisplayManager.h"

struct ClothCamera
{
	const float speed = 0.05f;
	const float frustumRatio = 1.0f;

	glm::vec3 pos = glm::vec3(0.0f, 4.0f, 12.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -2.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 uniProjMatrix;
	glm::mat4 uniViewMatrix;

	ClothCamera()
	{
		/** Projection matrix : The frustum that camera observes **/
		uniProjMatrix = glm::mat4(1.0f);
		uniProjMatrix = glm::perspective(glm::radians(45.0f), frustumRatio, 0.1f, 100.0f);
		/** View Matrix : The camera **/
		uniViewMatrix = glm::mat4(1.0f);
	}
};
ClothCamera cam;

struct ClothLight
{
	glm::vec3 pos = glm::vec3(-5.0f, 17.0f, 6.0f);
	glm::vec3 color = glm::vec3(0.7f, 0.7f, 1.0f);
};
ClothLight sun;

struct ClothRender // Texture & Lighting
{
	const Cloth* cloth;
	Loader loader;
	DisplayManager displayManager;
	int nodeCount; // Number of all nodes in faces

	glm::vec3 *vboPos; // 顶点
	glm::vec2 *vboTex; // 纹理
	glm::vec3 *vboNor; // 法线

	GLuint programID;
	GLuint vaoID;
	GLuint vboIDs[3];
	GLuint texID;
	GLuint emissionTexID;
	float time;

	GLint aPtrPos;
	GLint aPtrTex;
	GLint aPtrNor;

	ClothRender(Cloth *cloth, MasterRenderer &masterRenderer)
	{
		nodeCount = (int)(cloth->faces.size());
		if (nodeCount <= 0) {
			std::cout << "ERROR::ClothRender : No node exists." << std::endl;
			exit(-1);
		}

		this->cloth = cloth;

		vboPos = new glm::vec3[nodeCount];
		vboTex = new glm::vec2[nodeCount];
		vboNor = new glm::vec3[nodeCount];
		for (int i = 0; i < nodeCount; i++) {
			Node* n = cloth->faces[i];
			vboPos[i] = glm::vec3(n->position.x, n->position.y, n->position.z);
			vboTex[i] = glm::vec2(n->texCoord.x, n->texCoord.y); // Texture coord will only be set here
			vboNor[i] = glm::vec3(n->normal.x, n->normal.y, n->normal.z);
		}

		/** Build render program **/
		Program program("Shader/ClothVS.glsl", "Shader/ClothFS.glsl");
		programID = program.ID;

		// Generate ID of VAO and VBOs
		glGenVertexArrays(1, &vaoID);
		glGenBuffers(3, vboIDs);

		// Attribute pointers of VAO
		aPtrPos = 0;
		aPtrTex = 1;
		aPtrNor = 2;
		// Bind VAO
		glBindVertexArray(vaoID);

		// vbo顶点buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
		glVertexAttribPointer(aPtrPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, nodeCount * sizeof(glm::vec3), vboPos, GL_DYNAMIC_DRAW);
		// vbo纹理buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
		glVertexAttribPointer(aPtrTex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, nodeCount * sizeof(glm::vec2), vboTex, GL_DYNAMIC_DRAW);
		// Normal buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
		glVertexAttribPointer(aPtrNor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, nodeCount * sizeof(glm::vec3), vboNor, GL_DYNAMIC_DRAW);

		// Enable it's attribute pointers since they were set well
		glEnableVertexAttribArray(aPtrPos);
		glEnableVertexAttribArray(aPtrTex);
		glEnableVertexAttribArray(aPtrNor);

		/** Load texture **/
		texID = loader.loadTexture("box");
		emissionTexID = loader.loadTexture("lightbox");

		/** Set uniform **/
		glUseProgram(programID); // Active shader before set uniform
		// 加载纹理
		glUniform1i(glGetUniformLocation(programID, "uniTex"), 0);
		glUniform1i(glGetUniformLocation(programID, "emissionTex"), 1);

		/** 投影矩阵 : 矩形的投影矩阵 **/
		// Since projection matrix rarely changes, set it outside the rendering loop for only onec time
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniProjMatrix"), 1, GL_FALSE, glm::value_ptr(masterRenderer.getProjectionMatrix(true)));

		/** 光线 **/
		glUniform3fv(glGetUniformLocation(programID, "uniLightPos"), 1, &(sun.pos[0]));
		glUniform3fv(glGetUniformLocation(programID, "uniLightColor"), 1, &(sun.color[0]));

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbined VBO
		glBindVertexArray(0); // Unbined VAO
	}

	void destroy()
	{
		delete[] vboPos;
		delete[] vboTex;
		delete[] vboNor;

		if (vaoID)
		{
			glDeleteVertexArrays(1, &vaoID);
			glDeleteBuffers(3, vboIDs);
			vaoID = 0;
		}
		if (programID)
		{
			glDeleteProgram(programID);
			programID = 0;
		}
	}

	void flush(Camera &camera)
	{
		// Update all the positions of nodes
		for (int i = 0; i < nodeCount; i++) { // Tex coordinate dose not change
			Node* n = cloth->faces[i];
			vboPos[i] = glm::vec3(n->position.x, n->position.y, n->position.z);
			vboNor[i] = glm::vec3(n->normal.x, n->normal.y, n->normal.z);
		}

		glUseProgram(programID);

		glBindVertexArray(vaoID);

		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, nodeCount * sizeof(glm::vec3), vboPos);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, nodeCount * sizeof(glm::vec2), vboTex);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, nodeCount * sizeof(glm::vec3), vboNor);

		/** Bind texture **/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, emissionTexID);

		time = displayManager.getCurrentFrameTime();
		glUniform1f(glGetUniformLocation(programID, "time"), time);

		/** View Matrix : The camera **/
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniViewMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

		glm::mat4 uniModelMatrix = glm::mat4(1.0f);
		uniModelMatrix = glm::translate(uniModelMatrix, glm::vec3(cloth->clothPos.x, cloth->clothPos.y, cloth->clothPos.z));
		uniModelMatrix = glm::rotate(uniModelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		uniModelMatrix = glm::scale(uniModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniModelMatrix"), 1, GL_FALSE, &uniModelMatrix[0][0]);

		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/** Draw **/
		switch (cloth->drawMode) {
		case Cloth::DRAW_NODES:
			glDrawArrays(GL_POINTS, 0, nodeCount);
			break;
		case Cloth::DRAW_LINES:
			glDrawArrays(GL_LINES, 0, nodeCount);
			break;
		default:
			glDrawArrays(GL_TRIANGLES, 0, nodeCount);
			break;
		}

		// End flushing
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
};

struct SpringRender
{
	std::vector<Spring*> springs;
	int springCount; // Number of nodes in springs

	glm::vec4 uniSpringColor;

	glm::vec3 *vboPos; // Position
	glm::vec3 *vboNor; // Normal

	GLuint programID;
	GLuint vaoID;
	GLuint vboIDs[2];

	GLint aPtrPos;
	GLint aPtrNor;

	// Render any spring set, color and modelVector
	void init(std::vector<Spring*> s, glm::vec4 c, glm::vec3 modelVec)
	{
		springs = s;
		springCount = (int)(springs.size());
		if (springCount <= 0) {
			std::cout << "ERROR::SpringRender : No node exists." << std::endl;
			exit(-1);
		}

		uniSpringColor = c;

		vboPos = new glm::vec3[springCount * 2];
		vboNor = new glm::vec3[springCount * 2];
		for (int i = 0; i < springCount; i++) {
			Node* node1 = springs[i]->node1;
			Node* node2 = springs[i]->node2;
			vboPos[i * 2] = glm::vec3(node1->position.x, node1->position.y, node1->position.z);
			vboPos[i * 2 + 1] = glm::vec3(node2->position.x, node2->position.y, node2->position.z);
			vboNor[i * 2] = glm::vec3(node1->normal.x, node1->normal.y, node1->normal.z);
			vboNor[i * 2 + 1] = glm::vec3(node2->normal.x, node2->normal.y, node2->normal.z);
		}

		/** Build render program **/
		Program program("SpringVS.glsl", "SpringFS.glsl");
		programID = program.ID;
		std::cout << "Spring Program ID: " << programID << std::endl;

		// Generate ID of VAO and VBOs
		glGenVertexArrays(1, &vaoID);
		glGenBuffers(2, vboIDs);

		// Attribute pointers of VAO
		aPtrPos = 0;
		aPtrNor = 1;
		// Bind VAO
		glBindVertexArray(vaoID);

		// Position buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
		glVertexAttribPointer(aPtrPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, springCount * 2 * sizeof(glm::vec3), vboPos, GL_DYNAMIC_DRAW);
		// Normal buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
		glVertexAttribPointer(aPtrNor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, springCount * 2 * sizeof(glm::vec3), vboNor, GL_DYNAMIC_DRAW);

		// Enable it's attribute pointers since they were set well
		glEnableVertexAttribArray(aPtrPos);
		glEnableVertexAttribArray(aPtrNor);

		/** Set uniform **/
		glUseProgram(programID); // Active shader before set uniform
		// Set color
		glUniform4fv(glGetUniformLocation(programID, "uniSpringColor"), 1, &uniSpringColor[0]);

		/** Projection matrix : The frustum that camera observes **/
		// Since projection matrix rarely changes, set it outside the rendering loop for only onec time
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniProjMatrix"), 1, GL_FALSE, &cam.uniProjMatrix[0][0]);

		/** Model Matrix : Put rigid into the world **/
		glm::mat4 uniModelMatrix = glm::mat4(1.0f);
		uniModelMatrix = glm::translate(uniModelMatrix, modelVec);
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniModelMatrix"), 1, GL_FALSE, &uniModelMatrix[0][0]);

		/** Light **/
		glUniform3fv(glGetUniformLocation(programID, "uniLightPos"), 1, &(sun.pos[0]));
		glUniform3fv(glGetUniformLocation(programID, "uniLightColor"), 1, &(sun.color[0]));

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbined VBO
		glBindVertexArray(0); // Unbined VAO
	}

	void destroy()
	{
		delete[] vboPos;
		delete[] vboNor;

		if (vaoID)
		{
			glDeleteVertexArrays(1, &vaoID);
			glDeleteBuffers(2, vboIDs);
			vaoID = 0;
		}
		if (programID)
		{
			glDeleteProgram(programID);
			programID = 0;
		}
	}

	void flush(Camera &camera) // Rigid does not move, thus do not update vertexes' data
	{
		// Update all the positions of nodes
		for (int i = 0; i < springCount; i++) {
			Node* node1 = springs[i]->node1;
			Node* node2 = springs[i]->node2;
			vboPos[i * 2] = glm::vec3(node1->position.x, node1->position.y, node1->position.z);
			vboPos[i * 2 + 1] = glm::vec3(node2->position.x, node2->position.y, node2->position.z);
			vboNor[i * 2] = glm::vec3(node1->normal.x, node1->normal.y, node1->normal.z);
			vboNor[i * 2 + 1] = glm::vec3(node2->normal.x, node2->normal.y, node2->normal.z);
		}

		glUseProgram(programID);

		glBindVertexArray(vaoID);

		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, springCount * 2 * sizeof(glm::vec3), vboPos);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, springCount * 2 * sizeof(glm::vec3), vboNor);

		/** View Matrix : The camera **/
		cam.uniViewMatrix = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniViewMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/** Draw **/
		glDrawArrays(GL_LINES, 0, springCount * 2);

		// End flushing
		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
};

struct ClothSpringRender
{
	Cloth *cloth;
	glm::vec4 defaultColor;
	SpringRender render;

	ClothSpringRender(Cloth* c)
	{
		cloth = c;
		defaultColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
		render.init(cloth->springs, defaultColor, glm::vec3(cloth->clothPos.x, cloth->clothPos.y, cloth->clothPos.z));
	}

	void flush(Camera &camera) { render.flush(camera); }
};

struct RigidRender // Single color & Lighting
{
	std::vector<ClothVertex*> faces;
	int vertexCount; // Number of nodes in faces

	glm::vec4 uniRigidColor;

	glm::vec3 *vboPos; // Position
	glm::vec3 *vboNor; // Normal

	GLuint programID;
	GLuint vaoID;
	GLuint vboIDs[2];

	GLint aPtrPos;
	GLint aPtrNor;

	// Render any rigid body only with it's faces, color and modelVector
	void init(std::vector<ClothVertex*> f, glm::vec4 c, glm::vec3 modelVec, MasterRenderer &masterRenderer, Camera &camera)
	{
		faces = f;
		vertexCount = (int)(faces.size());
		if (vertexCount <= 0) {
			std::cout << "ERROR::RigidRender : No vertex exists." << std::endl;
			exit(-1);
		}

		uniRigidColor = c;

		vboPos = new glm::vec3[vertexCount];
		vboNor = new glm::vec3[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			ClothVertex* v = faces[i];
			vboPos[i] = glm::vec3(v->position.x, v->position.y, v->position.z);
			vboNor[i] = glm::vec3(v->normal.x, v->normal.y, v->normal.z);
		}

		/** Build render program **/
		Program program("Shader/RigidVS.glsl", "Shader/RigidFS.glsl");
		programID = program.ID;
		std::cout << "Rigid Program ID: " << programID << std::endl;

		// Generate ID of VAO and VBOs
		glGenVertexArrays(1, &vaoID);
		glGenBuffers(2, vboIDs);

		// Attribute pointers of VAO
		aPtrPos = 0;
		aPtrNor = 1;
		// Bind VAO
		glBindVertexArray(vaoID);

		// Position buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
		glVertexAttribPointer(aPtrPos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec3), vboPos, GL_DYNAMIC_DRAW);
		// Normal buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
		glVertexAttribPointer(aPtrNor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec3), vboNor, GL_DYNAMIC_DRAW);

		// Enable it's attribute pointers since they were set well
		glEnableVertexAttribArray(aPtrPos);
		glEnableVertexAttribArray(aPtrNor);

		/** Set uniform **/
		glUseProgram(programID); // Active shader before set uniform
		// Set color
		glUniform4fv(glGetUniformLocation(programID, "uniRigidColor"), 1, &uniRigidColor[0]);

		/** Projection matrix : The frustum that camera observes **/
		// Since projection matrix rarely changes, set it outside the rendering loop for only onec time
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniProjMatrix"), 1, GL_FALSE, glm::value_ptr(masterRenderer.getProjectionMatrix(true)));

		/** Model Matrix : Put rigid into the world **/
		glm::mat4 uniModelMatrix = glm::mat4(1.0f);
		uniModelMatrix = glm::translate(uniModelMatrix, glm::vec3(modelVec));
		uniModelMatrix = glm::rotate(uniModelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		uniModelMatrix = glm::scale(uniModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniModelMatrix"), 1, GL_FALSE, &uniModelMatrix[0][0]);

		/** Light **/
		glUniform3fv(glGetUniformLocation(programID, "uniLightPos"), 1, &(sun.pos[0]));
		glUniform3fv(glGetUniformLocation(programID, "uniLightColor"), 1, &(sun.color[0]));

		// Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbined VBO
		glBindVertexArray(0); // Unbined VAO
	}

	void destroy()
	{
		delete[] vboPos;
		delete[] vboNor;

		if (vaoID)
		{
			glDeleteVertexArrays(1, &vaoID);
			glDeleteBuffers(2, vboIDs);
			vaoID = 0;
		}
		if (programID)
		{
			glDeleteProgram(programID);
			programID = 0;
		}
	}

	void flush(Camera &camera) // Rigid does not move, thus do not update vertexes' data
	{
		glUseProgram(programID);

		glBindVertexArray(vaoID);

		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(glm::vec3), vboPos);
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(glm::vec3), vboNor);

		/** View Matrix : The camera **/
		glUniformMatrix4fv(glGetUniformLocation(programID, "uniViewMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/** Draw **/
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		// End flushing
		glDisable(GL_BLEND);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
};

struct GroundRender
{
	Ground *ground;
	RigidRender render;

	GroundRender(Ground* g, MasterRenderer &masterRenderer, Camera &camera)
	{
		ground = g;
		render.init(ground->faces, ground->color, glm::vec3(ground->position.x, ground->position.y, ground->position.z), masterRenderer, camera);
	}

	void flush(Camera &camera) { render.flush(camera); }
};

struct BallRender
{
	Ball* ball;
	RigidRender render;

	BallRender(Ball* b, MasterRenderer &masterRenderer, Camera &camera)
	{
		ball = b;
		render.init(ball->sphere->faces, ball->color, glm::vec3(ball->center.x, ball->center.y, ball->center.z), masterRenderer, camera);
	}

	void flush(Camera &camera) { render.flush(camera); }
};