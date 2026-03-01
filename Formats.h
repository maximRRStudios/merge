#ifndef _FORMATS_H_F56B5C28_1776_4113_A1BA_89B3022EBF42_
#define _FORMATS_H_F56B5C28_1776_4113_A1BA_89B3022EBF42_

#include "types.h"

class Formats {
public: // Common Methods
	virtual byte getFormat(void) = 0;
	virtual bool Create(FILE*, Header) = 0;
	virtual Header Open(FILE*, byte) = 0;
	virtual byte** Read(void) = 0;
	virtual void Write(byte**) = 0;
	virtual void Destroy(void) = 0;
};

#endif