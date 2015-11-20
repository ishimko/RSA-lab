#ifndef RSAWORKER_H
#define RSAWORKER_H

#include "types.h"
#include <QObject>

class RSAWorker : public QObject
{
	Q_OBJECT
public:
	explicit RSAWorker(QObject *parent = 0);

private:
	word fastModularExponentiation(word number, word power, word modulo);
	word euclidExtended(unsigned short a, int b, int *x, int *y);
	word getMultiplicativeInverse(word number, word modulo);

	bool isRunning;

};

#endif // RSAWORKER_H
