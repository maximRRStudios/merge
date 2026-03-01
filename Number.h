#ifndef _NUMBER_H_
#define _NUMBER_H_

#include <ctype.h>
#include "num_config.h"
#include "Image.h"

class Number {
	byte type;
	char* number;
	Image* source;
	Image* dest;
	bool triple;
public: // Constructors & Destructors
	Number(byte, Image*);
	~Number(void);
public: // Public Methods
	Image* Generate(char*);
	Number* setType(byte);
private: // Private Methods
	Image* GenerateRu(void);
	Image* GenerateEn(void);
	byte getIndexRu(char);
	byte getIndexEn(char);
};

#endif