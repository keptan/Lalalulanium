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
#include "nfun/future_pool.h" 
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

Color sample (const Ray& r, const Scene& s) 
{
	return s.sample(r);
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

/*
std::vector<std::unique_ptr<Hittable>> generateTrianglesPtr (const std::string v, const std::string& t)
{
	const auto verts = generateVerts(v);
	std::vector<std::unique_ptr<Hittable>> acc; 
	std::ifstream infile(t); 


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

		acc.emplace_back(std::make_unique<Triangle>(verts[std::stoi(record[0])],verts[std::stoi(record[1])],verts[std::stoi(record[2])]));
	}

	return acc;
}
*/

auto cameraTest (const Scene& scene)
{
	//image header, size and depth 
	const int width = 200;
	const int height  = 100;
	const int samples = 500;


	const Camera cam(Point(0, 0, 0)); 


	//operate over pixel positions
	const auto rb = Integers(height - 1 ,0) 
					| Product(Integers(0, width - 1)) 
					| Map([&](const auto tuple)
					{
					const auto [x, y] = tuple;
					const Color color =  (Integers() | Take(samples) | Fold([&](const auto s, const auto c)
					{
						const float v = x / float (height);
						const float u = y / float (width);
						const Ray r = cam.get_ray(u, v);
						const Color found = sample(r, scene);

						return c + found; 
					}, Color(0, 0, 0))).eval();


				//average 
					const Color average = color / samples; 
					const auto gammaCorrected = Color(std::sqrt(average.r()),std::sqrt(average.g()),std::sqrt(average.b()));

					const int ir = int(255.99 * gammaCorrected.r());
					const int ig = int(255.99 * gammaCorrected.g());
					const int ib = int(255.99 * gammaCorrected.b());

					std::cout << ir << ' ' << ig << ' ' << ib << '\n';
					});


	eval(rb);
}

std::vector<std::tuple<std::tuple<int, int>, Color>> scanBatch (const Scene& scene, const Camera& cam , std::tuple<int, int> boxs, std::tuple<int, int> boxe, std::tuple<int, int> image)
{
	std::vector<std::tuple<std::tuple<int, int>, Color>> acc; 
	const auto [height, width] = boxs; 
	const auto [hEnd, wEnd]	   = boxe;
	const auto [xE, yE] = image;

	const int samples = 500;


	const auto rb = Integers(hEnd - 1 ,height) 
					| Product(Integers(width, wEnd - 1)) 
					| Map([&](const auto tuple)
					{
					const auto [x, y] = tuple;
					const Color color =  (Integers() | Take(samples) | Fold([&](const auto s, const auto c)
					{
						const float v = x / float (xE);
						const float u = y / float (yE);
						const Ray r = cam.get_ray(u, v);
						const Color found = sample(r, scene);

						return c + found; 
					}, Color(0, 0, 0))).eval();


				//average 
				// 
					//std::cout << x << ' ' << y << std::endl;
					const Color average = color / samples; 
					const auto gammaCorrected = Color(std::sqrt(average.r()),std::sqrt(average.g()),std::sqrt(average.b()));
					std::tuple<std::tuple<int, int>, Color> out {{x, y}, gammaCorrected};
					acc.push_back(out);
					});


	eval(rb);

	return acc;
}

auto cameraBatch (const Scene& scene) 
{

	const int width  = 200;
	const int height = 100; 
	const int samples = 500; 
	const Camera cam(Point(0, 0, 0));

	const int rowsPer = std::min(height / 8, (height / ((height * width) / 10000)));

	int rowsLeft = height; 
	int batchNum = 0;


	FutureDad manDad(8);

	std::vector<std::future<std::vector<std::tuple<std::tuple<int, int>, Color>>>> acc((height / rowsPer + 1));
	while(rowsLeft - rowsPer > 0)
	{

		const auto run = ([=, &scene, &cam]()
				{
					const auto out = scanBatch(scene, cam, {rowsLeft - rowsPer, 0}, {rowsLeft, width}, {height, width});
					std::cerr << " finished batch!" << std::endl;

					return out;
				});

		acc[batchNum] = manDad.addTask(run);
		rowsLeft = rowsLeft - rowsPer; 
		batchNum++;
	}
	if(rowsLeft > 0)
	{
		acc[batchNum] = manDad.addTask([&](){return scanBatch(scene, cam, {0, 0}, {rowsLeft, width}, {height, width});});
	}

	std::cout << "P3\n" << width << ' ' << height << "\n255\n";

	for( auto& v : acc)
	{
		v.wait();
		for(auto& t : v.get()) 
		{
			const auto [p, gammaCorrected] = t; 
			const auto [x, y] = p; 

			const int ir = int(255.99 * gammaCorrected.r());
			const int ig = int(255.99 * gammaCorrected.g());
			const int ib = int(255.99 * gammaCorrected.b());

			std::cout << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}
}




int main (void)
{
	std::shared_ptr<Material> red = std::make_shared<Lambertian>(Color(0.8, 0.5, 0.5));
	std::shared_ptr<Material> green = std::make_shared<Lambertian>(Color(0.52, 1.0, 0.5));
	std::shared_ptr<Material> gray = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
//	imperativePPM();
//	outputPPM();
	Scene scene; 
	scene.actors.emplace_back(buildSculpture(generateTriangles("verts.v", "triangles.v")));
//	scene.actors = generateTrianglesPtr("verts.v", "triangles.v");
	scene.actors.emplace_back(std::make_unique<Sphere>(Point(0, -100.5, -1), 100, gray));
	scene.actors.emplace_back(std::make_unique<Sphere>(Point(-2, 0, -1), 0.5, red));
	//scene.actors.emplace_back(std::make_unique<Sphere>(Point(0, 0, -1), 0.5));

//cameraTest(scene);
cameraBatch(scene);


}
