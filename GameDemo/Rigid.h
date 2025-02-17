﻿#pragma once

#include <math.h>

#include <cmath>
#include <vector>

#include "Points.h"

struct Ground
{
    Vec3 position;
    int width, height;
    glm::vec4 color;
    const double friction = 0.9;
    
    std::vector<ClothVertex*> vertexes;
    std::vector<ClothVertex*> faces;
    
    Ground(Vec3 pos, Vec2 size, glm::vec4 c) {
        position = pos;
        width = size.x;
        height = size.y;
        color = c;
        
        init();
    }
    ~Ground()
    {
        for (int i = 0; i < vertexes.size(); i++) { delete vertexes[i]; }
        vertexes.clear();
        faces.clear();
    }
    
    void init()
    {
        vertexes.push_back(new ClothVertex(Vec3(0.0, 0.0, 0.0)));
        vertexes.push_back(new ClothVertex(Vec3(width, 0.0, 0.0)));
        vertexes.push_back(new ClothVertex(Vec3(0.0, 0.0, -height)));
        vertexes.push_back(new ClothVertex(Vec3(width, 0.0, -height)));
        
        for (int i = 0; i < vertexes.size(); i ++) {
            vertexes[i]->normal = Vec3(0.0, 1.0, 0.0); // It's not neccessery to normalize here
            
            // Debug info
            printf("Ground[%d]: (%f, %f, %f) - (%f, %f, %f)\n", i, vertexes[i]->position.x, vertexes[i]->position.y, vertexes[i]->position.z, vertexes[i]->normal.x, vertexes[i]->normal.y, vertexes[i]->normal.z);
        }
        
        faces.push_back(vertexes[0]);
        faces.push_back(vertexes[1]);
        faces.push_back(vertexes[2]);
        faces.push_back(vertexes[1]);
        faces.push_back(vertexes[2]);
        faces.push_back(vertexes[3]);
    }
};

class Sphere
{
public:
    const int meridianNum = 24;
    const int parallelNum = 250;
    
    int radius;
    
    std::vector<ClothVertex*> vertexes;
    std::vector<ClothVertex*> faces;
    
    Sphere(int r)
    {
        radius = r;
        init();
    }
    ~Sphere()
    {
        for (int i = 0; i < vertexes.size(); i++) { delete vertexes[i]; }
        vertexes.clear();
        faces.clear();
    }
    
	ClothVertex* getTop() { return vertexes[0]; }
	ClothVertex* getVertex(int x, int y)
    {
        if (x < 0 || x >= parallelNum || y < 0 || y >= meridianNum) {
            printf("Vertex Index Out of Range.\n");
            exit(-1);
        } else {
            return vertexes[1+x*meridianNum+y];
        }
    }
	ClothVertex* getBottom() { return vertexes[vertexes.size()-1]; }
    
    Vec3 computeFaceNormal(ClothVertex* v1, ClothVertex* v2, ClothVertex* v3)
    {
        return Vec3::cross(v2->position - v1->position, v3->position - v1->position);
    }
    
    void computeSphereNormal()
    {
        Vec3 normal(0.0, 0.0, 0.0);
        for (int i = 0; i < vertexes.size(); i ++) {
            vertexes[i]->normal = normal;
        }
        
        // The normal of all faces of the first and last cycle should be calculated specially!
        for (int i = 0; i < faces.size()/3; i ++) {
			ClothVertex* v1 = faces[i*3+0];
			ClothVertex* v2 = faces[i*3+1];
			ClothVertex* v3 = faces[i*3+2];
            
            normal = computeFaceNormal(v1, v3, v2);
            v1->normal += normal;
            v2->normal += normal;
            v3->normal += normal;
        }
        
        for (int i = 0; i < vertexes.size(); i ++) {
            vertexes[i]->normal.normalize();
        }
    }
    
    void init() // Initialize vertexes coord and slice faces
    {
        /** Compute vertex position **/
        double cycleInterval = radius*2.0 / (parallelNum+1);
        //double radianInterval = 2.0*M_PI/meridianNum;
		double radianInterval = 2.0* 3.1415926 / meridianNum;
        
        
        Vec3 pos(0.0, radius, 0.0);
        vertexes.push_back(new ClothVertex(pos)); // Top vertex
        
        for (int i = 0; i < parallelNum; i ++) {
            pos.y -= cycleInterval;
            for (int j = 0; j < meridianNum; j ++) {
                double xzLen = radius * sqrt(1.0 - pow(pos.y/radius, 2));
                double xRadian = j * radianInterval;  // The length of projection line on X-Z pane
                
                pos.x = xzLen * sin(xRadian);
                pos.z = xzLen * cos(xRadian);
                vertexes.push_back(new ClothVertex(pos));
            }
        }
        pos = Vec3(0.0, -radius, 0.0);
        vertexes.push_back(new ClothVertex(pos)); // Bottom vertex
        
        /** Slice faces **/
        // Top cycle
        for (int i = 0; i < meridianNum; i ++) {
            faces.push_back(getVertex(0, i));                               //   *   //
            faces.push_back(getTop());                                      //  / \  //
            faces.push_back(getVertex(0, (i+1)%meridianNum));               // *---* //
        }
        // Middle cycles
        for (int i = 0; i < parallelNum-1; i ++) {
            for (int j = 0; j < meridianNum; j ++) {
                faces.push_back(getVertex(i, j));                           //  *--* //
                faces.push_back(getVertex(i, (j+1)%meridianNum));           //  | /  //
                faces.push_back(getVertex(i+1, j));                         //  *    //
                
                faces.push_back(getVertex(i+1, (j+1)%meridianNum));         //     * //
                faces.push_back(getVertex(i+1, j));                         //   / | //
                faces.push_back(getVertex(i, (j+1)%meridianNum));           //  *--* //
            }
        }
        // Bottom cycle
        for (int i = 0; i < meridianNum; i ++) {
            faces.push_back(getBottom());                                   // *---* //
            faces.push_back(getVertex(parallelNum-1, i));                   //  \ /  //
            faces.push_back(getVertex(parallelNum-1, (i+1)%meridianNum));   //   *   //
        }
        
        /** Set normals **/
        computeSphereNormal();
    }
};

struct Ball
{
	//球的中心点
	Vec3 center;
	//球的半径
	int radius;
	//球的颜色
	glm::vec4 color;
	//球的阻力
	const double friction = 0.8;

	//球体类的实例化
	Sphere* sphere;

	//球类的构造函数
	Ball(Vec3 cen, int r, glm::vec4 c)
	{
		center = cen;
		radius = r;
		color = c;

		//球体类 参数是半径
		sphere = new Sphere(radius);
	}
	~Ball() {}
};
