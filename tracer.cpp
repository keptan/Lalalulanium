#include "fun/generators.h"
#include "fun/list.h" 
#include "fun/string.h" 
#include "fun/sort.h"
#include "fun/tree.h"
#include "fun/range.h" 
#include "tVector.h"
#include "materials.h"
#include "hits.h" 
#include "camera.h"
#include "scene.h" 
#include <cmath>
#include <future>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <sstream> 
#include <fstream> 
#include <memory>


float sphereCollision (const Point& center, float radius, const Ray& r)
{
	Point oc = r.origin() - center; 
	float a = r.direction().dot(r.direction()); 
	float b = 2.0 * oc.dot(r.direction()); 
	float c = oc.dot(oc) - radius * radius; 
	float disc = b*b - 4*a*c;

	if(disc < 0) return -1.0;

	return (-b - std::sqrt(disc)) / (2.0 * a);
}

std::vector<Point> generateVerts (const std::string& filename)
{
	std::vector<Point> acc; 

	std::ifstream infile(filename); 


	while(infile)
	{
		std::string s; 
		if(!getline(infile, s)) break; 

		std::istringstream ss (s); 
		std::vector<std::string> record; 
		while(ss) 
		{
			std::string s; 
			if(!getline(ss, s, ',')) break; 
			record.push_back(s); 
		}

		acc.push_back( Point( std::stof(record[0]),std::stof(record[1]),std::stof(record[2])));
	}

	return acc;
}

std::vector<Triangle> generateTriangles (const std::string v, const std::string& t)
{
	const auto verts = generateVerts(v);
	std::vector<Triangle> acc; 
	std::ifstream infile(t); 
	std::shared_ptr<Material> green = std::make_shared<Lambertian>(Color(0, 0.5, 0));

	while(infile)
	{
		std::string s; 
		if(!getline(infile, s)) break; 

		std::istringstream ss (s); 
		std::vector<std::string> record; 
		while(ss) 
		{
			std::string s; 
			if(!getline(ss, s, ',')) break; 
			record.push_back(s); 
		}

		acc.emplace_back(verts[std::stoi(record[0])],verts[std::stoi(record[1])],verts[std::stoi(record[2])], green);
	}

	return acc;
}


int main (void)
{
	std::shared_ptr<Material> red = std::make_shared<Lambertian>(Color(0.8, 0.5, 0.5));
	std::shared_ptr<Material> green = std::make_shared<Lambertian>(Color(0.52, 1.0, 0.5));
	std::shared_ptr<Material> gray = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
	Scene scene; 
	const Camera c (Point(0, 0, 0));

	scene.actors.emplace_back(buildSculpture(generateTriangles("verts.v", "triangles.v")));
	scene.actors.emplace_back(std::make_unique<Sphere>(Point(0, -100.5, -1), 100, gray));
	scene.actors.emplace_back(std::make_unique<Sphere>(Point(-2, 0, -1), 0.5, red));

	scene.batchRender(c, 200, 100, 100);


}
