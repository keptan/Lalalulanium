#include "fun/list.h" 
#include "fun/string.h" 
#include "fun/sort.h"
#include "fun/tree.h"
#include "fun/range.h" 
#include "tVector.h"
#include "hits.h" 
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
	Sphere s (Point(0, 0, -1), 0.5); 
	Sphere earth (Point(0, -100.5, -1), 100)};

	if(const auto hit = s.hit(r,0, 1000) )
	{
		Point normal = hit->normal;
		return 0.5 * Color(normal.x() + 1, normal.y()+1, normal.z()+1);
	}
;
	if(const auto hit = earth.hit(r,0, 1000) )
	{
		Point normal = hit->normal;
		return 0.5 * Color(normal.x() + 1, normal.y()+1, normal.z()+1);
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
	const Point lowerLeft (-2, -1, -1); 
	const Point right		(4, 0, 0); 
	const Point vertical  (0, 2, 0); 
	const Point origin    (0, 0, 0);

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

					const float v = x / float (height);
					const float u = y / float (width);
					const Ray r (origin, lowerLeft + u * right + v * vertical);
					const Color c = background(r);


					const int ir = int(255.99 * c.r());
					const int ig = int(255.99 * c.g());
					const int ib = int(255.99 * c.b());

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
