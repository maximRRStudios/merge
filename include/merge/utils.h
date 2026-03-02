#ifndef _UTILS_H_63AEA884_67B1_44f3_A6E3_7198F44A50FE_
#define _UTILS_H_63AEA884_67B1_44f3_A6E3_7198F44A50FE_

#include <stdlib.h>
#include <merge/types.h>
#include <merge/Exception.h>
#include <math.h>

#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

#define ERROR_FOPEN		0x100
#define ERROR_MALLOC	0x200

#ifdef __GNUC__
#define DEPRECATED(func, msg) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func, msg) __declspec(deprecated(msg)) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func, msg) func
#endif

inline size_t timestamp(void) {
#ifdef WIN32
	return (size_t)time(0);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (size_t)tv.tv_sec;
#endif
}

inline ushort ushort_rand(ushort max)
	{ return (ushort)((double)rand() * (double)max / (double)RAND_MAX); }

template<typename Type>
inline Type MIN(Type a, Type b)
	{ return (a < b) ? a : b; }

template<typename Type>
inline Type MAX(Type a, Type b)
	{ return (a > b) ? a : b; }

template<typename Type>
inline Type ABS(Type a)
	{ return (a < 0) ? -a : a; }

inline int Roundup(double a) {
	if (ABS<double>(a - (int)(a + 5e-10)) < 1e-9)
		return (int)(a + 5e-10);
	else return (int)(a + 1);
}

inline int Round(double a)
	{ return (int)(a + 0.5); }

template<typename Type>
inline Type** AllocateMemory(size_t num_rows, size_t num_cols) {
	Type** rows = (Type**)malloc(sizeof(Type*) * num_rows);
	if (rows == NULL)
		throw Exception(ERROR_MALLOC, "Could not allocate memory");
	for (size_t i = 0; i < num_rows; i++) {
		rows[i] = (byte*)malloc(sizeof(byte) * num_cols);
		if(rows[i] == NULL) {
			while(--i >= 0) if (rows[i]) free(rows[i]);
			free(rows);
			throw Exception(ERROR_MALLOC, "Could not allocate memory");
		}
		memset(rows[i], 0xFF, num_cols * sizeof(Type));
	}
	return rows;
}

inline double COS(double degrees) {
	double off = (degrees / 30.0 - Round(degrees / 30.0));
	if (off < .0000001 && off > -.0000001) {
		int idegrees = (int)Round(degrees);
		idegrees = (idegrees < 0) ? (360 - (-idegrees % 360)) : (idegrees % 360);
		switch (idegrees) {
			case   0:	return  1.0;
			case  30:	return  0.866025403784439;
			case  60:	return  0.5;
			case  90:	return  0.0;
			case 120:	return -0.5;
			case 150:	return -0.866025403784439;
			case 180:	return -1.0;
			case 210:	return -0.866025403784439;
			case 240:	return -0.5;
			case 270:	return  0.0;
			case 300:	return  0.5;
			case 330:	return  0.866025403784439;
			case 360:	return  1.0;
			default:	return  cos(degrees * 3.14159265358979 / 180.0);
		}
	} else return cos(degrees * 3.14159265358979 / 180.0);
}

inline double SIN(double degrees) {
	return COS(degrees + 90.0);
}

#endif