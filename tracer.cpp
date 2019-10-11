#include "fun/list.h" 
#include "fun/string.h" 
#include "fun/sort.h"
#include "fun/tree.h"
#include "fun/range.h" 
#include <iostream>


auto outputPPM (void)
{
	//image header, size and depth 
	const int width = 200;
	const int height  = 100;

	std::cout << "P3\n" << width << ' ' << height << "\n255\n";

	//operate over pixel positions
	const auto rb = Integers(0, height - 1) 
					| Product(Integers(0, width - 1)) 
					| Map([&](const auto tuple)
					{
					const auto distance =  [&](const auto x1, const auto y1, const auto x2, const auto y2) -> float
					{
							return std::sqrt(std::pow(y2 -y1, 2) + std::pow(x2 - x1, 2));
					};
						
					const auto [x, y] = tuple;
					const float r = x / float (height);
					const float g = distance(x, y, 0, width) / distance(height, 0, 0, width);
					const float b = 1 - (distance(x, y, 0, width) / distance(height, 0, 0, width));

					const int ir = int(255.99 * r);
					const int ig = int(255.99 * g);
					const int ib = int(255.99 * b);

					std::cout << ir << ' ' << ig << ' ' << ib << '\n';
					});


	eval(rb);
}


int main (void)
{
	outputPPM();
}
