#include "rsaworker.h"

RSAWorker::RSAWorker(QObject *parent):
	QObject(parent),
	isRunning(false)
{
}

word RSAWorker::fastModularExponentiation(word number, word power, word modulo)
{
	word result = 1;

	while (number){
		while (!(power % 2)){
			power /= 2;
			number = ((uint32)number * number) % modulo;
		}
		power--;
		result = ((uint32)result * number) % modulo;
	}
	return result;
}

word RSAWorker::getMultiplicativeInverse(word number, word modulo)
{
	int x, y;
	euclidExtended(number, modulo, &x, &y);

	return (x % modulo + modulo) % modulo;
}

word RSAWorker::euclidExtended(unsigned short a, int b, int *x, int *y){
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


