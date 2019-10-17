#pragma once 
#include "hits.h"
#include "tVector.h"
#include "nfun/future_pool.h" 
#include "materials.h"
#include "camera.h"

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
			const auto scatter = hit->mat->scatter(r, *hit);
			if(!scatter) return Color(0, 0, 0);

			const auto [scattered, attenuation] = *scatter;

			return attenuation * sample(scattered, depth + 1);
		}

		Point unit_direction = r.direction().unitVector(); 
		float t = 0.5 * (unit_direction.y() + 1.0); 
		return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
	}

	std::vector<std::tuple<std::tuple<int, int>, Color>> scanBatch ( const Camera& cam , std::tuple<int, int> boxs, std::tuple<int, int> boxe, std::tuple<int, int> image)
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
							const Color found = this->sample(r);

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

	void batchRender (const Camera& cam, const int width = 200, const int height = 100, const int samples = 100) 
	{
		const int rowsPer = std::min(height / 8, (height / ((height * width) / 10000)));

		int rowsLeft = height; 
		int batchNum = 0;


		FutureDad manDad(8);

		std::vector<std::future<std::vector<std::tuple<std::tuple<int, int>, Color>>>> acc((height / rowsPer + 1));
		while(rowsLeft - rowsPer > 0)
		{

			const auto run = ([=, &cam]()
					{
						const auto out = scanBatch(cam, {rowsLeft - rowsPer, 0}, {rowsLeft, width}, {height, width});
						std::cerr << " finished batch!" << std::endl;

						return out;
					});

			acc[batchNum] = manDad.addTask(run);
			rowsLeft = rowsLeft - rowsPer; 
			batchNum++;
		}
		if(rowsLeft > 0)
		{
			acc[batchNum] = manDad.addTask([&](){return scanBatch(cam, {0, 0}, {rowsLeft, width}, {height, width});});
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
};
		
