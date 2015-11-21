#include "rsaroutines.h"

word fastModularExponentiation(word number, word power, word modulo)
{
	uint32 result = 1;

	while (power){
		while (!(power % 2)){
			power = power / 2;
			number = (number * number) % modulo;
		}
		power--;
		result = (result * number) % modulo;
	}
	return result;
}

word getMultiplicativeInverse(word number, word modulo)
{
	int x, y;
	euclidExtended(number, modulo, &x, &y);

	return (x % modulo + modulo) % modulo;
}

word euclidExtended(unsigned short a, int b, int *x, int *y){
	int x1, y1;
	unsigned short d;
	if (a == 0){
		*x = 0;
		*y = 1;
		return b;
	}

	d = euclidExtended(b % a, a, &x1, &y1);
	*x = y1 - (b / a) * x1;
	*y = x1;
	return d;
}
