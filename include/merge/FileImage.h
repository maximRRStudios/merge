#ifndef _FILEIMAGE_H_48F151A7_49F6_4b46_9F54_3C950B4D570F_
#define _FILEIMAGE_H_48F151A7_49F6_4b46_9F54_3C950B4D570F_

#include <merge/utils.h>
#include "PngImage.h"
#include "JpgImage.h"

class FileImage {
	FILE* file;
	byte access;
	Header info;
	Formats* out;
public:
	FileImage(void);
	~FileImage(void);
public:
	bool Create(char*, Header);
	Header Open(char*, byte);
	byte** Read(void);
	void Write(byte**);
	void Destroy(void);
	void Close(void);
};

#endif