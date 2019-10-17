#pragma once
#include "tVector.h" 
#include <cmath>
#include <cstddef>
#include <optional>
#include <algorithm>
#include <map>

class Material;

struct Hit 
{
	//time hit 
	float t; 
	Point p; 
	Point normal; 
	std::shared_ptr<Material> mat;
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
		std::shared_ptr<Material> mat;

		Sphere (void) 
		{}

		Sphere (const Point c, const float r, std::shared_ptr<Material> m) 
			: center (c), radius (r), mat(m)
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
								(r.point_at_t(temp) - center) / radius, mat};
				}
			}

			return std::nullopt; 
		}
};

class Triangle : public Hittable 
{

	public:  
		Point a, b, c;  
		std::shared_ptr<Material> mat;

		Triangle (void) 
		{}

		Triangle(const Point a, const Point b, const Point c, std::shared_ptr<Material> m) 
			: a(a), b(b), c(c), mat(m)
		{}

		virtual std::optional<Hit> hit 
		(const Ray& r, float t_min, float t_max) const final 
		{
			const float EPSILON = 0.0000001;
			const auto edge1 = b - a; 
			const auto edge2 = c - a; 
			const auto h = r.direction().cross(edge2); 
			const auto a_ = edge1.dot(h); 

			if (a_ > -EPSILON && a_ < EPSILON) return std::nullopt; 

			const float f = 1/a_; 
			const Point  s = r.origin() - a; 
			const float u = f * s.dot(h); 

			if( u < 0.0 || u > 1.0) return std::nullopt; 

			const auto q = s.cross(edge1); 
			const auto v = f * r.direction().dot(q); 

			if( v < 0.0 || u + v > 1.0) return std::nullopt; 

			float t = f * edge2.dot(q); 
			if( t > t_min && t < t_max) 
			{
				return Hit {t, r.point_at_t(t), edge1.cross(edge2), mat};
			}

			return std::nullopt; 
		}
};

struct RecBound : public Hittable 
{
		Point a, b; 
		std::vector<std::shared_ptr<Hittable>> target;

		RecBound (const Point a, const Point b,  const std::vector<std::shared_ptr<Hittable>> t)
			: a(a), b(b), target(t)
		{}

		bool hitBound
		(const Ray& r, float tmin, float tmax) const
		{
			const float dir[] =    {std::get<0>(r.direction().res), std::get<1>(r.direction().res),std::get<2>(r.direction().res)};
			const float origin[] = {std::get<0>(r.origin().res), std::get<1>(r.origin().res),std::get<2>(r.origin().res)};

			const float min[] = {std::get<0>(a.res), std::get<1>(a.res), std::get<2>(a.res)};
			const float max[] = {std::get<0>(b.res), std::get<1>(b.res), std::get<2>(b.res)};

			for(int a = 0; a < 3; a++)
			{
				float invD = 1.0f / dir[a];
				float t0 = (min[a] - origin[a]) * invD; 
				float t1 = (max[a] - origin[a]) * invD; 

				if(invD < 0.0f) std::swap(t0, t1); 

				tmin = t0 > tmin ? t0 : tmin; 
				tmax = t1 < tmax ? t1 : tmax; 

				if (tmax <= tmin) return false; 
			}

			return true; 
		}

		virtual std::optional<Hit> hit 
		(const Ray& r, float t_min, float t_max) const final
		{
			if(! hitBound(r, t_min, t_max)) return std::nullopt; 
			std::optional<Hit> hit; 
			double closest = t_max; 
			for(const auto& a : target)
			{
				if(const auto h = a->hit(r, t_min, closest))
				{
					hit = *h;
					closest = h->t; 
				}
			}

			return hit;
		}

};

class Sculpture : public Hittable 
{

	public:  
		std::vector<Triangle> triangles; 

		Sculpture (const  std::vector<Triangle>& t)
			: triangles(t)
		{}

	
		virtual std::optional<Hit> hit 
		(const Ray& r, float t_min, float t_max) const final
		{
			std::optional<Hit> hit; 
			double closest = t_max; 
			for(const auto& a : triangles)
			{
				if(const auto h = a.hit(r, t_min, closest))
				{
					hit = *h;
					closest = h->t; 
				}
			}

			return hit;
		}
};

std::tuple<Point, Point> triangleBounds (const std::vector<Triangle>& triangles)
{
	float minx = triangles[0].a.x(); 
	float miny = triangles[0].a.y(); 
	float minz = triangles[0].a.z(); 

	float maxx = triangles[0].a.x(); 
	float maxy = triangles[0].a.y(); 
	float maxz = triangles[0].a.z(); 

	for(const auto& t : triangles)
	{
		const auto test = [&](const auto ti)
		{
			if(ti.x() > maxx) maxx = ti.x();
			if(ti.y() > maxy) maxy = ti.y();
			if(ti.z() > maxz) maxz = ti.z();

			if(ti.x() < minx) minx = ti.x();
			if(ti.y() < miny) miny = ti.y();
			if(ti.z() < minz) minz = ti.z();

		};

		test(t.a);
		test(t.b);
		test(t.c);
	}

	return std::make_tuple(Point(minx, miny, minz), Point(maxx, maxy, maxz));
}


std::unique_ptr<Hittable> buildSculpture (const std::vector<Triangle>& triangles, int depth = 0)
{
	if (depth > 10 || triangles.size() < 10) return std::make_unique<Sculpture>(triangles);

	const auto [low, high] = triangleBounds(triangles);
	const auto getMid = [](const auto a, const auto b){ return a + (b - a)/2;};
	const Point mid(getMid(low.x(), high.x()),getMid(low.z(), high.z()),getMid(low.z(), high.z()));

	std::cerr << low.x() << ' ' << mid.x() << ' ' << high.x() << std::endl;

	std::vector<Triangle> a1, b1, c1, d1, a2, b2, c2, d2;
	for(const auto& t : triangles)
	{
		/*  *C*
		 *
		 * b a
		 * c d */ 

		const auto tBound = [](const auto t){ return Point( std::max(t.a.x(), std::max(t.b.x(), t.c.x())), std::max(t.a.y(), std::max(t.b.y(), t.c.y())),std::max(t.a.z(), std::max(t.b.z(), t.c.z())));};
		const auto big = tBound(t);

		//what if we just calculated the  distance to the center of each of these bounds or something LMAO!
		const bool up = big.z() >= mid.z(); 
		const bool far = big.y() >= mid.y();
		const bool right = big.x() >= mid.x();

		//essentially un unrolled binary search to stick the triangle into the correct sub-rect
		if(up)
		{
			if(far)
			{
				if(right) 
				{
					c1.push_back(t);
					continue;
				}
				d1.push_back(t);
				continue;
			}
			if(right)
			{
				a1.push_back(t);
				continue;
			}
			b1.push_back(t);
			continue;
		}
			if(far)
			{
				if(right) 
				{
					c2.push_back(t);
					continue;
				}
				d2.push_back(t);
				continue;
			}
			if(right)
			{
				a2.push_back(t);
				continue;
			}
			b2.push_back(t);
			continue;
	}

	std::vector<std::shared_ptr<Hittable>> nest;
	nest.push_back(buildSculpture(a1, depth + 1));
	nest.push_back(buildSculpture(b1, depth + 1));
	nest.push_back(buildSculpture(c1, depth + 1));
	nest.push_back(buildSculpture(d1, depth + 1));
	nest.push_back(buildSculpture(a2, depth + 1));
	nest.push_back(buildSculpture(b2, depth + 1));
	nest.push_back(buildSculpture(c2, depth + 1));
	nest.push_back(buildSculpture(d2, depth + 1));

	return std::make_unique<RecBound>(low, high, nest);
}

				

