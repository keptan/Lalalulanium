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
					| Map([](const auto tuple)
					{
					const auto [x, y] = tuple;
					const float r = ((float) x / (float) height);
					const float g = ((float) y / (float) width);
					const float b = ((std::sqrt(std::pow(y - width/2, 2) + std::pow(x - height/2, 2))) / 
									std::sqrt( std::pow(height/2, 2) + std::pow(width/2, 2)));

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
