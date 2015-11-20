#include "rsaworker.h"

RSAWorker::RSAWorker()
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

