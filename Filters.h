#ifndef _FILTERS_H_FB1096E7_4C38_4b35_8390_4B9DAB2F7AB2_
#define _FILTERS_H_FB1096E7_4C38_4b35_8390_4B9DAB2F7AB2_

#include "utils.h"
#include "Layer.h"
#include "Exception.h"
#include <string.h>
#include <math.h>

// Resize Filters
#define NEAREST			0x01
#define BILINEAR		0x02
#define BICUBIC			0x04

// Rotation Node Struct
struct aar_indll {
    aar_indll* next;
    int ind;
};

class Filters {
public: // Public Methods
	static void Resize(Layer*, size_t, byte);
	static void BilinearResize(Layer*, size_t, size_t);
private: // Private Methods
};

#endif