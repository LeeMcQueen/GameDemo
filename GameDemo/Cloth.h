﻿#pragma once

#include <vector>

#include "Spring.h"
#include "Rigid.h"

class Cloth
{
public:
	const int nodesDensity = 4;
	const int iterationFreq = 25;
	const double structuralCoef = 1000.0;
	const double shearCoef = 50.0;
	const double bendingCoef = 400.0;

	enum DrawModeEnum {
		DRAW_NODES,
		DRAW_LINES,
		DRAW_FACES
	};
	DrawModeEnum drawMode = DRAW_FACES;

	Vec3 clothPos;

	int width, height;
	int nodesPerRow, nodesPerCol;
	int _width, _height, _colorChannels;

	const float MAX_HEIGHT = 40.0f;
	const float MIN_HEIGHT = -40.0f;
	const float MAX_PIXEL_COLOUR = 256 * 256 * 256;

	unsigned char *_image = stbi_load("res/heightmap4.png", &_width, &_height, &_colorChannels, 0);

	std::vector<Node*> nodes;
	std::vector<Spring*> springs;
	std::vector<Node*> faces;

	Vec2 pin1;
	Vec2 pin2;

	Cloth(Vec3 pos, Vec2 size)
	{
		clothPos = pos;
		width = size.x;
		height = size.y;
		init();
	}
	~Cloth()
	{
		for (int i = 0; i < nodes.size(); i++) { delete nodes[i]; }
		for (int i = 0; i < springs.size(); i++) { delete springs[i]; }
		nodes.clear();
		springs.clear();
		faces.clear();
	}

public:
	//根据
	Node* getNode(int x, int y) { return nodes[y*nodesPerRow + x]; }

	Vec3 computeFaceNormal(Node* n1, Node* n2, Node* n3)
	{
		return Vec3::cross(n2->position - n1->position, n3->position - n1->position);
	}

	void pin(Vec2 index, Vec3 offset) // Pin cloth's (x, y) node with offset
	{
		if (!(index.x < 0 || index.x >= nodesPerRow || index.y < 0 || index.y >= nodesPerCol)) {
			getNode(index.x, index.y)->position += offset;
			getNode(index.x, index.y)->isFixed = true;
		}
	}
	void unPin(Vec2 index) // Unpin cloth's (x, y) node
	{
		if (!(index.x < 0 || index.x >= nodesPerRow || index.y < 0 || index.y >= nodesPerCol)) {
			getNode(index.x, index.y)->isFixed = false;
		}
	}

	void init()
	{
		nodesPerRow = width * nodesDensity;
		nodesPerCol = height * nodesDensity;

		pin1 = Vec2(0, 0);
		pin2 = Vec2(nodesPerRow - 1, 0);

		/** Add nodes **/
		//printf("Init cloth with %d nodes\n", nodesPerRow*nodesPerCol);
		for (int j = 0; j < nodesPerCol; j++) {
			for (int i = 0; i < nodesPerRow; i++) {
				/** Create node by position **/
				Node* node = new Node(Vec3(0, (double)j / nodesDensity * 5, (double)i / nodesDensity * 5));
				/** Set texture coordinates **/
				node->texCoord.x = (double)j / (nodesPerRow - 1);
				node->texCoord.y = (double)i / (1 - nodesPerCol);
				/** Add node to cloth **/
				nodes.push_back(node);

				//节点信息输出功能
				/*printf("\t[%d, %d] (%f, %f, %f) - (%f, %f)\n", i, j, node->position.x, node->position.y, node->position.z, node->texCoord.x, node->texCoord.y);*/
			}
			std::cout << std::endl;
		}

		/** Add springs **/
		for (int i = 0; i < nodesPerRow; i++) {
			for (int j = 0; j < nodesPerCol; j++) {
				/** Structural **/
				if (i < nodesPerRow - 1) springs.push_back(new Spring(getNode(i, j), getNode(i + 1, j), structuralCoef));
				if (j < nodesPerCol - 1) springs.push_back(new Spring(getNode(i, j), getNode(i, j + 1), structuralCoef));
				/** Shear **/
				if (i < nodesPerRow - 1 && j < nodesPerCol - 1) {
					springs.push_back(new Spring(getNode(i, j), getNode(i + 1, j + 1), shearCoef));
					springs.push_back(new Spring(getNode(i + 1, j), getNode(i, j + 1), shearCoef));
				}
				/** Bending **/
				if (i < nodesPerRow - 2) springs.push_back(new Spring(getNode(i, j), getNode(i + 2, j), bendingCoef));
				if (j < nodesPerCol - 2) springs.push_back(new Spring(getNode(i, j), getNode(i, j + 2), bendingCoef));
			}
		}

		//节点的间隔，和碰撞相关
		pin(pin1, Vec3(0.2, 0.0, 0.0));
		pin(pin2, Vec3(-0.0, 0.0, 0.0));

		/** Triangle faces **/
		for (int j = 0; j < nodesPerCol - 1; j++) {
			for (int i = 0; i < nodesPerRow - 1; i++) {
				// Left upper triangle
				faces.push_back(getNode(i + 1, j));
				faces.push_back(getNode(i, j));
				faces.push_back(getNode(i, j + 1));
				// Right bottom triangle
				faces.push_back(getNode(i + 1, j + 1));
				faces.push_back(getNode(i + 1, j));
				faces.push_back(getNode(i, j + 1));
			}
		}
	}

	void computeNormal()
	{
		/** Reset nodes' normal **/
		Vec3 normal(0.0, 0.0, 0.0);
		for (int i = 0; i < nodes.size(); i++) {
			nodes[i]->normal = normal;
		}
		/** Compute normal of each face **/
		for (int i = 0; i < faces.size() / 3; i++) { // 3 nodes in each face
			Node* n1 = faces[3 * i + 0];
			Node* n2 = faces[3 * i + 1];
			Node* n3 = faces[3 * i + 2];

			// Face normal
			normal = computeFaceNormal(n1, n2, n3);
			// Add all face normal
			n1->normal += normal;
			n2->normal += normal;
			n3->normal += normal;
		}

		for (int i = 0; i < nodes.size(); i++) {
			nodes[i]->normal.normalize();
		}
	}

	void addForce(Vec3 f)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->addForce(f);
		}
	}

	void computeForce(double timeStep, Vec3 gravity)
	{
		/** Nodes **/
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->addForce(gravity * nodes[i]->mass);
		}
		/** Springs **/
		for (int i = 0; i < springs.size(); i++)
		{
			springs[i]->applyInternalForce(timeStep);
		}
	}

	void integrate(double airFriction, double timeStep)
	{
		/** Node **/
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->integrate(timeStep);
		}
	}

	Vec3 getWorldPos(Node* n) {
		return clothPos + (n->position) ;
	}

	void setWorldPos(Node* n, Vec3 pos) {
		n->position = pos - clothPos;
	}

	float getHeight(int x, int z, unsigned char *data) {
	
		if (x < 0 || x > _height || z < 0 || z > _height) {
			return 0;
		}

		float height = getRGBSum(x, z);
		height /= (MAX_PIXEL_COLOUR / 2);
		height -= 1.0;
		height *= MAX_HEIGHT;

		return height;
	}

	std::int32_t getRGBSum(int x, int y) {
	
		int addr = (y * _width + x) * _colorChannels;
		std::int32_t r = _image[addr];
		std::int32_t g = _image[addr + 1];
		std::int32_t b = _image[addr + 2];
		return (r << 16) + (g << 8) + b;
	}

	void collisionResponse(Vec3 ball)
	{
		//循环布料的全部节点List
		for (int i = 0; i < nodes.size(); i++)
		{
			Vec3 nodesPos = getWorldPos(nodes[i]);
			/** 地面碰撞 **/
			//节点的高度小于地面的高度就抬高
			if (getWorldPos(nodes[i]).y < getHeight(getWorldPos(nodes[i]).x, getWorldPos(nodes[i]).z, _image) + 5.0f) {
				nodes[i]->position.y = getHeight(getWorldPos(nodes[i]).x, getWorldPos(nodes[i]).z, _image) + 5.0f - clothPos.y + 0.01;
				//节点的速度 = 节点的速度 * 地面的阻力
				nodes[i]->velocity = nodes[i]->velocity * 0.5f;
			}

			/** 球体碰撞 **/
			//节点到球体的距离
			Vec3 distVec = getWorldPos(nodes[i]) - ball;
			//距离值
			double distLen = distVec.length();
			//碰撞距离 球的半径ball->radius
			double safeDist = 5.0 * 1.05;
			if (distLen < safeDist) {
				distVec.normalize();
				Vec3 newPos = distVec*safeDist;
				setWorldPos(nodes[i], distVec*safeDist + ball);
				//球的摩擦力是 ball->friction
				nodes[i]->velocity = nodes[i]->velocity*0.8f;
			}
		}
	}
};
