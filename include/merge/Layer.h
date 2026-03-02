#ifndef _LAYER_H_B8447F42_7CBF_4e93_AD2F_F9E6DEBAB727_
#define _LAYER_H_B8447F42_7CBF_4e93_AD2F_F9E6DEBAB727_

#include <stdlib.h>
#include <merge/utils.h>

struct RGBA {
	byte	red,
			green,
			blue,
			alpha;
};

struct LayerLoc {
	__int32 x;
	__int32 y;
	byte z;
};

class Layer {
public:
	byte** rows;
	Header info;
	LayerLoc position;
	size_t bg_color;
	byte flags;
public: // Constructors & Destructors
	Layer(Header, byte**, LayerLoc);
	~Layer(void);
public: // Set/Get Pixels
	void set_pixel_rgb (size_t, size_t, size_t);
	void set_pixel_argb(size_t, size_t, size_t);
	void set_pixel_rgba(size_t, size_t, size_t);
	size_t get_pixel_argb(size_t, size_t);
	RGBA get_pixel_channels(size_t, size_t);
public: // Public Methods
	void ChangeHeader(Header);
	void SetData(byte**);
	void Delete();
	byte** Copy(size_t, size_t, size_t, size_t);
public: // Read Only Properties
	byte** getImage(void);
	Header getInfo(void);
	LayerLoc getLoc(void);
	size_t getBgColor(void);
	byte getBgByChannel(byte);
	byte getFlags(void);
};

#endif