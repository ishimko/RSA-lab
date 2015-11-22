#ifndef RSAROUTINES
#define RSAROUTINES

#include "types.h"

#include <math.h>
#include <QFile>

word fastModularExponentiation(word number, word power, word modulo);
word euclidExtended(word a, int b, int *x, int *y);
word getMultiplicativeInverse(word number, word modulo);
bool isPrime(word number);
word gcd(word a, word b);

word getFirstDivider(word number);

void writeDecipheredByte(word out, QFile *outputFile);
void writeCipheredWord(word out, QFile *outputFile);

word readCipheredWord(QFile *inputFile);
word readPlaintextByte(QFile *inputFile);

#endif // RSAROUTINES

