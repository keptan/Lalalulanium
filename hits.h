#pragma once
#include "tVector.h" 
#include <cmath>
#include <cstddef>
#include <optional>

struct Hit 
{
	//time hit 
	float t; 
	Point p; 
	Point normal; 
}; 

class Hittable 
{
	public: 
		virtual std::optional<Hit> hit 
		(const Ray& r, float t_min, float t_max) const = 0; 
};

class Sphere : public Hittable 
{
		
	public: 
		Point center; 
		float radius; 

		Sphere (void) 
		{}

		Sphere (const Point c, const float r) 
			: center (c), radius (r)
		{}

		virtual std::optional<Hit> hit 
		(const Ray& r, float t_min, float t_max) const final
		{
			Point oc = r.origin() - center; 
			float a = r.direction().dot(r.direction()); 
			float b = oc.dot(r.direction()); 
			float c = oc.dot(oc) - radius * radius; 

			float discriminant = b*b - a*c;  

			if(discriminant > 0) 
			{
				float temp = (-b - std::sqrt(discriminant)) / a;  

				if( temp < t_max && temp > t_min) 
				{
					return Hit {temp, r.point_at_t(temp), 
								(r.point_at_t(temp) - center) / radius};
				}
			}

			return std::nullopt; 
		}
};





