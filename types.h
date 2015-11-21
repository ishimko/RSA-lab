#ifndef TYPES_H
#define TYPES_H

typedef unsigned short word;
typedef unsigned long uint32;
typedef unsigned char byte;

typedef enum{
	E_NOT_PRIME_P,
	E_NOT_PRIME_Q,
	E_NOT_PRIME,
	E_TOO_BIG_P_Q,
	E_INVALID_KEY,
	E_TOO_BIG_KEY
} ErrorType;

#endif // TYPES_H

