#include <tuple>
#include <math.h>

#include <utility>
template<typename T, size_t N>
struct TVec
{

	template<size_t, class I>
	using T0T_ = I;

	template<class I, size_t... Is>
	static auto tGen(std::index_sequence<Is...>) { return std::tuple<T0T_<Is, I>...>{}; }

	template<class I, size_t IN>
	static auto tGen() { return tGen<I>(std::make_index_sequence<IN>{}); }

	using Tuple = decltype(tGen<T, N>());
	using Index = std::make_index_sequence<N>;
	
	Tuple res; 


	template<typename F, std::size_t... I>
	constexpr Tuple elementWise (const Tuple& a, const Tuple& b, F f, std::index_sequence<I...> = Index{})
	{
		return std::make_tuple( f(std::get<I>(a),std::get<I>(b))... );
	}


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

	T  operator [] (const int i) const {return std::get<i>(res);}
	//T& operator [] (const int i) {return std::get<i>(res);}

	TVec operator + (const TVec& b)
	{
		const auto add = [](const auto a, const auto b){ return a + b;};
		Tuple out = elementWise(res, b.res, add, Index{});
		return TVec{out};
	}
};
