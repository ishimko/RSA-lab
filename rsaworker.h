#ifndef RSAWORKER_H
#define RSAWORKER_H

#include "types.h"

class RSAWorker
{
public:
	RSAWorker();

private:
	word fastModuloExpansion(word number, word degree, word modulo);
};

#endif // RSAWORKER_H
