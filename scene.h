#pragma once 
#include "hits.h"
#include "tVector.h"

class Scene 
{
	public:
	std::vector<std::unique_ptr<Hittable>> actors; 

	//tests if a ray hits anything in the scene between tMin and tMax travel distance 
	//returns 
	std::optional<Hit> shoot (const Ray& r, const float tMin, const float tMax) const
	{
		std::optional<Hit> hit; 
		double closest = tMax; 
		for(const auto& a : actors)
		{
			if(const auto h = a->hit(r, tMin, closest))
			{
				hit = *h;
				closest = h->t; 
			}
		}

		return hit; 
	}

	//bounches rays around the scene until it returns a color 
	Color sample (const Ray& r, int depth = 0) const
	{
		const auto hit = shoot(r, 0.001, MAXFLOAT);
		if(depth < 25 && hit)
		{
			Point target = hit->p + hit->normal + random_unit(); 
			return 0.5 * sample(Ray(hit->p, target - hit->p), depth + 1); 
		}

		Point unit_direction = r.direction().unitVector(); 
		float t = 0.5 * (unit_direction.y() + 1.0); 
		return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
	}
};
		
