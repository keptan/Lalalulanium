#pragma once 
#include <cstddef>
#include <tuple>
#include <math.h>

#include <utility>
#include "utility.h"
template<typename T, size_t N>
class TVec
{

	template<size_t, class I>
	using T0T_ = I;

	template<class I, size_t... Is>
	static auto tGen(std::index_sequence<Is...>) { return std::tuple<T0T_<Is, I>...>{}; }

	template<class I, size_t IN>
	static auto tGen() { return tGen<I>(std::make_index_sequence<IN>{}); }

	using Tuple = decltype(tGen<T, N>());
	using Index = std::make_index_sequence<N>;

	template<typename F, std::size_t... I>
	constexpr Tuple elementWise (const Tuple& a, const Tuple& b, F f, std::index_sequence<I...> = Index{}) const
	{
		return std::make_tuple( f(std::get<I>(a),std::get<I>(b))... );
	}


	public:
	
	Tuple res; 



	const TVec& operator+(void) const 
	{
		return *this;
	}

	TVec operator -(void) const 
	{
		Tuple out = res;
		const auto minus = [](auto& a){ a = -a;};

		//apply minus lambda to all tuple members...
		std::apply([&](auto&&... args) {(minus(args) , ...);}, out);
		return TVec{out};
	}

	//T  operator [] (const int i) const {return std::get<i>(res);}
	//T& operator [] (const int i) {return std::get<i>(res);}

	TVec operator + (const TVec& b) const
	{
		return  TVec{elementWise(res, b.res, [](const auto a, const auto b){return a + b;}, Index{})};
	}

	TVec operator - (const TVec& b) const
	{
		return  TVec{elementWise(res, b.res, [](const auto a, const auto b){return a - b;}, Index{})};
	}

	TVec operator * (const TVec& b) const
	{
		return  TVec{elementWise(res, b.res, [](const auto a, const auto b){return a * b;}, Index{})};
	}

	TVec operator / (const TVec& b) const
	{
		return  TVec{elementWise(res, b.res, [](const auto a, const auto b){return a / b;}, Index{})};
	}

	TVec operator * (const T& b) const
	{
		Tuple out = res;
		const auto minus = [&](auto& a){ a = a * b;};

		//apply minus lambda to all tuple members...
		std::apply([&](auto&&... args) {(minus(args) , ...);}, out);
		return TVec{out};
	}

	TVec operator / (const T& b) const
	{
		Tuple out = res;
		const auto minus = [&](auto& a){ a = a / b;};

		//apply minus lambda to all tuple members...
		std::apply([&](auto&&... args) {(minus(args) , ...);}, out);
		return TVec{out};
	}

	TVec unitVector (void) const
	{
		Tuple out = res;
		T acc = 0;
		const auto accSqr = [&](const auto& a){ acc += a * a;};
		std::apply([&](auto&&... args) {(accSqr(args) , ...);}, out);

		//std::cout << acc << std::endl;

		T k = 1.0 / std::sqrt(acc);

		const auto mK = [&](auto& a){ a *= k;};
		std::apply([&](auto&&... args) {(mK(args), ...);}, out);

		return TVec{out};
	}

	T dot (const TVec& b) const 
	{
		TVec acc = *this * b;  
		T s = 0;
		const auto sum = [&](const auto& a){ s += a;};
		std::apply([&](auto&&... args) {(sum(args) , ...);}, acc.res);

		return s; 
	}

	T length (void) const 
	{
		Tuple out = res;
		T acc = 0;
		const auto accSqr = [&](const auto& a){ acc += a * a;};
		std::apply([&](auto&&... args) {(accSqr(args) , ...);}, out);
		return  std::sqrt(acc);
	}

	TVec cross (const TVec& b) const 
	{
		if constexpr(N == 3)
		{  
			return TVec{ std::make_tuple( 
						std::get<1>(res) * std::get<2>(b.res) - std::get<2>(res) * std::get<1>(b.res), 
						std::get<2>(res) * std::get<0>(b.res) - std::get<0>(res) * std::get<2>(b.res), 
						std::get<0>(res) * std::get<1>(b.res) - std::get<1>(res) * std::get<0>(b.res))};
		}
		else 
			static_assert(N == 3, "cross only defined for 3d vectors");
	}

};

template<typename T, size_t N>
TVec<T, N> operator * (const float a, const TVec<T, N>& b)
{
	return b * a; 
}

class Color : public TVec<float, 3>
{ 
	public:
	Color (float r, float g, float b)
		: TVec{std::make_tuple(r, g, b)}
	{}

	Color (const TVec& v)
		: TVec(v)
	{}

	float r (void) const
	{
		return std::get<0>(this->res);
	}

	float g (void) const
	{
		return std::get<1>(this->res);
	}

	float b (void) const
	{
		return std::get<2>(this->res);
	}
};

Color operator * (const float a, const Color& b)
{
	return b * a; 
}

class Point : public TVec<float, 3>
{ 
	public:
	Point (float x, float y, float z)
		: TVec{std::make_tuple(x, y, z)}
	{}

	Point (const TVec& v)
		: TVec(v)
	{}

	Point (void) 
	{}

	float x (void) const
	{
		return std::get<0>(this->res);
	}

	float y (void) const
	{
		return std::get<1>(this->res);
	}

	float z (void) const
	{
		return std::get<2>(this->res);
	}
};

Point operator * (const float a, const Point& b)
{
	return b * a; 
}

class Ray 
{
	public: 
	const Point A; 
	const Point B; 

	Ray (void) {}; 
	Ray (const Point& a, const Point& b)
		: A(a), B(b)
	{}

	Point origin (void) const 
	{
		return A; 
	}

	Point direction (void) const 
	{
		return B;  
	}

	Point point_at_t (float t) const 
	{
		return A + t * B;  
	}
};

Point random_unit (void) 
{
	Point p;
	do
	{
		p = 2.0 * Point(random_double(), random_double(), random_double()) - Point(1, 1, 1); 
	} while(p.length() * p.length() >= 1.0); 

	return p;  
}


