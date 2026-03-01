#ifndef _JPGIMAGE_H_
#define _JPGIMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "Formats.h"

class JpgImage : public Formats {
	FILE* file;
	byte access_type;
	Header header;
public:
	JpgImage(void);
	~JpgImage(void);
public: // Common Methods
	byte getFormat(void);
	bool Create(FILE*, Header);
	Header Open(FILE*);
	byte** Read(void);
	void Write(byte**);
	void Destroy(void);
private: // Private Methods
	void InitVars(void);
};

#endif