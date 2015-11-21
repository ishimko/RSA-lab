#ifndef TYPES_H
#define TYPES_H

#include <QFile>

typedef unsigned short word;
typedef unsigned long uint32;
typedef unsigned char byte;

typedef enum{
	E_NOT_PRIME_P,
	E_NOT_PRIME_Q,
	E_TOO_BIG_P_Q,
	E_INVALID_KEY,
	E_TOO_BIG_KEY,
	E_TOO_SMALL_P_Q
} ErrorType;

typedef enum{
	MODE_CIPHER,
	MODE_DECIPHER
} WorkerMode;

typedef void (*ResultWriter)(word, QFile*);
typedef word (*InputReader)(QFile*);

#endif // TYPES_H

