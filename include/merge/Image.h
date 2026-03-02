#ifndef _IMAGE_H_2C6B3087_2E11_481b_A119_F9DBA50CD5F2_
#define _IMAGE_H_2C6B3087_2E11_481b_A119_F9DBA50CD5F2_

#include <merge/utils.h>
#include <merge/FileImage.h>
#include <merge/Layer.h>
#include <merge/Filters.h>
#include <merge/num_config.h>

// Flatten Filters
#define FROM_IMAGE		0x01
#define COLOR_MAP		0x02
#define FILL_COLOR		0x04
#define COPY_ALPHA		0x08
#define GREYSCALE_MAP	0x10
#define CROP_BACKGROUND	0x20
#define EMBED_IN_BCKG	0x40

// [1..255]; Default	0xC8
#define CHANNEL_STRIP	0xFF
// Highlight; Default	0.70
#define HIGHLIGHT		0.65
// Avg Color; Default	0x80
#define AVERAGE_COLOR	0x80
// Visual Grey Color	0x9F
#define MEDIUM_CONTRAST	0x9F

class Image {
	byte count;
	size_t avg_grey;
	Layer** layers;
	Layer* bg;
public: // Constructors & Destructors
	Image(void);
	~Image(void);
public: //
	byte GetLayersCount(void);
	bool IsImageExists(void);
	Layer* GetLayer(byte);
	Layer* GetBackground(void);
public: // Public Methods
	void Clear(void);
	void Create(Header);
	void Create(size_t, size_t, short);
	void AddBackground(char* filename, byte flags, size_t color);
	void AddBackground(byte** rows, byte flags, size_t color, Header header);
	byte AddLayer(char* filename, size_t x, size_t y, byte z, byte flags, size_t color);
	byte AddLayer(byte** rows, Header header, size_t x, size_t y, byte z, byte flags, size_t color);
	Image* Flatten(void);
	byte** GenerateMap(size_t);
	Header GenerateHeader(size_t, size_t, short);
	void Save(char*);
private: // Private Methods
	byte LayerToGreyscale(byte layer_index);
	inline size_t GetGreyscale(size_t r, size_t g, size_t b);
	inline size_t LineGreyscale(size_t r, size_t g, size_t b);
	void CreateBackground(void);
};

#endif