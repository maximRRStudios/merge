#ifndef _TYPES_H_7C432C3E_7FB6_48b4_BDDF_0C7131ECDADA_
#define _TYPES_H_7C432C3E_7FB6_48b4_BDDF_0C7131ECDADA_

#define FILE_NULL		0x0
#define FILE_READ		0x1
#define FILE_WRITE		0x2
#define FILE_BINARY		0x4

#define FORMAT_PNG		0x1
#define FORMAT_JPG		0x2

#ifndef BYTE_DEFINED
#define BYTE_DEFINED
typedef unsigned char byte;
#endif

#ifndef USHORT_DEFINED
#define USHORT_DEFINED
typedef unsigned short ushort;
#endif

#ifndef WIN32
typedef int __int32;
#include <stddef.h>
#endif

#define BIN__N(x) (x) | x>>3 | x>>6 | x>>9
#define BIN__B(x) (x) & 0xf | (x)>>12 & 0xf0
#define BIN8(v) (BIN__B(BIN__N(0x##v)))
#define BIN16(x1,x2) ((BIN8(x1)<<8)+BIN8(x2))
#define BIN24(x1,x2,x3) ((BIN8(x1)<<16)+(BIN8(x2)<<8)+BIN8(x3))
#define BIN32(x1,x2,x3,x4) ((BIN8(x1)<<24)+(BIN8(x2)<<16)+(BIN8(x3)<<8)+BIN8(x4))

template<typename Type> struct Point {
	Type x, y;
	inline Point(void) {}
    inline Point(Type x, Type y): x(x), y(y) {}
};

#ifndef HEADER_STRUCT_DEFINED
#define HEADER_STRUCT_DEFINED
struct Header {
	bool	hidepth;
	short	bit_depth,
			color_type,
			interlace_type,
			compression_type,
			filter_type;
	size_t	width,
			height,
			row_bytes;
};
#endif

#endif