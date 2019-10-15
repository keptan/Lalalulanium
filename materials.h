#pragma once
#include "tVector.h"
#include "hits.h"

struct Material
{
	virtual std::optional<std::tuple<Ray, Color>>
	scatter (const Ray& r, const Hit& hit) = 0;
};


class Lambertian : public Material
{
	Color albedo; 

	public: 
		Lambertian (const Color a)
			:albedo(a)
		{}

		virtual std::optional<std::tuple<Ray, Color>>
		scatter (const Ray& r, const Hit& hit) final 
		{
			const auto target = hit.p + hit.normal + random_unit(); 
			const Ray scattered = Ray(hit.p, target - hit.p);

			return std::make_tuple(scattered, albedo);
		}
};

