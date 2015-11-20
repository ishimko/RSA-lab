#ifndef RSAWORKER_H
#define RSAWORKER_H

#include "types.h"

class RSAWorker
{
public:
	RSAWorker();

private:
	word fastModularExponentiation(word number, word power, word modulo);
	word euclidExtended(unsigned short a, int b, int *x, int *y);
	word getMultiplicativeInverse(word number, word modulo);
};

#endif // RSAWORKER_H
