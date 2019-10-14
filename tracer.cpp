#include "fun/list.h" 
#include "fun/string.h" 
#include "fun/sort.h"
#include "fun/tree.h"
#include "fun/range.h" 
#include "tVector.h"
#include "hits.h" 
#include "camera.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <math.h>



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

Color background (const Ray& r) 
{
	std::vector<Sphere> spheres {
	Sphere (Point(0, 0, -1), 0.5), 
	Sphere (Point(-4, -1, -1), 2.5),
	Sphere (Point(0, -100.5, -1), 100)};

	const auto worldHit = [&](const auto& r, const float tMin, const float tMax)
	{
		std::optional<Hit> ghit; 
		double closest = tMax;
		for(const auto& s : spheres) 
		{
			if(const auto h = s.hit(r, tMin, closest))
			{
				ghit = *h; 
				closest = h->t;
			}
		}
		return ghit; 
	};

	if(const auto hit = worldHit(r,0, MAXFLOAT))
	{
		Point target = hit->p + hit->normal + random_unit(); 
		return 0.5 * background(Ray(hit->p, target - hit->p));
	}
	/*
	if(t > 0.0) 
	{
		Point normal = (r.point_at_t(t) - Point(0, 0, -1)).unitVector();
	}
	*/


	Point unit_direction = r.direction().unitVector();
	float t = 0.5 * (unit_direction.y() + 1.0); 
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

auto cameraTest (void)
{
	//image header, size and depth 
	const int width = 200;
	const int height  = 100;
	std::cout << "P3\n" << width << ' ' << height << "\n255\n";

	Camera cam(Point(0, 0, 0)); 

	//operate over pixel positions
	const auto rb = Integers(height - 1 ,0) 
					| Product(Integers(0, width - 1)) 
					| Map([&](const auto tuple)
					{
					const auto distance =  [&](const auto x1, const auto y1, const auto x2, const auto y2) -> float
					{
							return std::sqrt(std::pow(y2 -y1, 2) + std::pow(x2 - x1, 2));
					};
						
					const auto [x, y] = tuple;
					Color col(0, 0, 0); 
					for(int s = 0; s < 100; s++)
					{
						const float v = x / float (height);
						const float u = y / float (width);
						const Ray r = cam.get_ray(u, v);
						const Color c = background(r);
						col =  col + c;
					}
					col = col / 100; 

					//GAMMA CORRECT!
					col = Color(std::sqrt(col.r()),std::sqrt(col.g()),std::sqrt(col.b()));
	

					const int ir = int(255.99 * col.r());
					const int ig = int(255.99 * col.g());
					const int ib = int(255.99 * col.b());

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
