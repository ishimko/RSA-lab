#ifndef RSAROUTINES
#define RSAROUTINES

#include "types.h"

word fastModularExponentiation(word number, word power, word modulo);
word euclidExtended(unsigned short a, int b, int *x, int *y);
word getMultiplicativeInverse(word number, word modulo);

#endif // RSAROUTINES

