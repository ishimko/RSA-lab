#ifndef RSAROUTINES
#define RSAROUTINES

#include "types.h"
#include <math.h>

word fastModularExponentiation(word number, word power, word modulo);
word euclidExtended(unsigned short a, int b, int *x, int *y);
word getMultiplicativeInverse(word number, word modulo);
bool isPrime(int number);
word gcd(word a, word b);

#endif // RSAROUTINES

