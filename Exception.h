#ifndef _EXCEPTION_H_77441574_9544_4bd9_BF0F_939548F9FEB2_
#define _EXCEPTION_H_77441574_9544_4bd9_BF0F_939548F9FEB2_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Exception {
	size_t code;
	char* message;
public:
	Exception(void);
	Exception(size_t);
	Exception(size_t, char*);
	Exception(size_t, char*, char*);
	~Exception(void);
	size_t getCode(void);
	char* getMessage(void);
};

#endif