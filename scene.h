#pragma once 
#include "hits.h"
#include "tVector.h"

class Scene 
{
	public:
	std::vector<std::unique_ptr<Hittable>> actors; 

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

	Color sample (const Ray& r) const
	{
		if(const auto hit = shoot(r, 0, MAXFLOAT))
		{
			Point target = hit->p + hit->normal + random_unit(); 
			return 0.5 * sample(Ray(hit->p, target - hit->p)); 
		}

		Point unit_direction = r.direction().unitVector(); 
		float t = 0.5 * (unit_direction.y() + 1.0); 
		return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
	}
};
		
