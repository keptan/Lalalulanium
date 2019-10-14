#pragma once
#include <cstdlib>

double random_double ()
{
	return rand() / (RAND_MAX + 1.0); 
}
