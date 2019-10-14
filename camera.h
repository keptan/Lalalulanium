#pragma once 
#include "utility.h"
#include "tVector.h"



struct Camera 
{
	Point origin; 
	Point lower_left_corner; 
	Point horizintal ;
	Point vertical; 

	Camera(const Point& o)
		: origin(o), lower_left_corner(o + Point(-2, -1, -1)), 
		horizintal(o + Point(4.0, 0.0, 0.0)),  vertical(o + Point(0,2, 0)) 
	{}

	Ray get_ray (float u, float v) 
	{
		return Ray(origin, lower_left_corner + u * horizintal + v*vertical - origin); 
	}
};


	



