#include "fun/list.h" 
#include "fun/string.h" 
#include "fun/sort.h"
#include "fun/tree.h"
#include "fun/range.h" 
#include "tVector.h"
#include "hits.h" 
#include "camera.h"
#include "scene.h" 
#include <cmath>
#include <iostream>
#include <algorithm>
#include <math.h>

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

Color sample (const Ray& r) 
{
	Scene scene; 
	scene.actors.emplace_back( std::make_unique<Sphere>(Point(-1, 0, -1), 0.5));
	//scene.actors.emplace_back( std::make_unique<Sphere>(Point(1, 0, -1), 0.5));
	//scene.actors.emplace_back( std::make_unique<Sphere>(Point(1, 1, -1), 0.5));
	scene.actors.emplace_back( std::make_unique<Sphere>(Point(0, -100.5, -1), 100));
	scene.actors.emplace_back( std::make_unique<Triangle>(
				Point(0.5, 0.0, -2),
				Point(-0.5, 0.0, -2),
				Point(1.0, 1.0, -1.5)

				));

	return scene.sample(r);
}

auto cameraTest (void)
{
	//image header, size and depth 
	const int width = 200;
	const int height  = 100;
	const int samples = 500;
	std::cout << "P3\n" << width << ' ' << height << "\n255\n";


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
						const Color found = sample(r);

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

int main (void)
{
//	imperativePPM();
//	outputPPM();
cameraTest();

}
