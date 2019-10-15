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

class Triangle : public Hittable 
{

	public:  
		Point a, b, c;  

		Triangle (void) 
		{}

		Triangle(const Point a, const Point b, const Point c) 
			: a(a), b(b), c(c) 
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
				return Hit {t, r.point_at_t(t), edge1.cross(edge2)};
			}

			return std::nullopt; 
		}
};

class Sculpture : public Hittable 
{

	public:  
		std::vector<Triangle> triangles; 
		Point a, b;

		Sculpture (const  std::vector<Triangle>& t)
			: triangles(t)
		{
			calculateBounds();
		}

		void calculateBounds (void)
		{
			if(!triangles.size()) return; 

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
					std::cerr << ti.x() << std::endl;
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
			
			std::cerr << minx << ' ' << miny << ' ' << minz << std::endl;
			std::cerr << maxx << ' ' << maxy << ' ' << maxz << std::endl;

			a = Point(minx, miny, minz); 
			b = Point(maxx, maxy, maxz);
		}


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









