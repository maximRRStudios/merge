#ifndef _PNGIMAGE_H_CD641C7D_3732_4a47_AD0B_27DA8EB54BA0_
#define _PNGIMAGE_H_CD641C7D_3732_4a47_AD0B_27DA8EB54BA0_

#include <stdio.h>
#include <stdlib.h>
#include "libpng/png.h"
#include "utils.h"
#include "Formats.h"
#include "Exception.h"

struct PngIhdr {
	__int32	bit_depth,
			color_type,
			interlace_type,
			compression_type,
			filter_type;
	png_uint_32 width,
				height;
};

struct PngChrm {
	png_fixed_point	white_x,
					white_y,
					red_x,
					red_y,
					green_x,
					green_y,
					blue_x,
					blue_y;
};

struct PngIccp {
	png_charp name;
	png_bytep profile;
	png_uint_32 proflen;
	__int32 compression_type;
};

struct PngPalette {
	png_colorp palette;
	__int32 num_palette;
};

struct PngBackground {
	png_color_16p background;
};

struct PngOffsets {
	png_int_32	offset_x,
				offset_y;
	__int32 unit_type;
};

struct PngPcal {
	png_charp	purpose,
				units;
	png_charpp params;
	png_int_32	X0,
				X1;
	__int32 type,
			nparams;
};

struct PngPhys {
	png_uint_32 res_x,
				res_y;
	__int32 unit_type;
};

struct PngScal {
	__int32 unit;
	double	scal_width,
			scal_height;
};

struct PngText {
	png_textp text_ptr;
	__int32 num_text;
};

struct PngTrans {
	png_bytep trans_alpha;
	__int32 num_trans;
	png_color_16p trans_color;
};

struct PngInfo {
	png_uint_32 height;
	png_uint_32 width;
	size_t row_bytes;
};

class PngImage : public Formats {
	FILE* file;
	byte access_type;
	size_t num_pass;
	Header header;
private: // PNG Structures
	png_structp png_ptr;
	png_infop info_ptr;
	png_infop end_info_ptr;
public: // Constructors & Destructors
	PngImage(void);
	~PngImage(void);
public: // Common Methods
	byte getFormat(void);
	bool Create(FILE*, Header);
	Header Open(FILE*, byte);
	byte** Read(void);
	void Write(byte**);
	void Destroy(void);
private: // Transforms
	void TransformToRGBA(void);
	void TransformSwapAlpha(void);
private: // Private Methods
	void ReadInfo(void);
	void WriteInfo(void);
	void ReadEndInfo(void);
	void WriteEndInfo(void);
	void InitFromFile(PngImage*);
	void EndFromFile(PngImage*);
	void SetInterlace(void);
	void InitVars(void);
	void CreatePngStruct(void);
private: // PNG Properties
	PngIhdr getPngIhdr(void);
	void setPngIhdr(PngIhdr);
	PngChrm getPngChrm(void);
	void setPngChrm(PngChrm);
	png_fixed_point getPngGamma(void);
	void setPngGamma(png_fixed_point);
	PngIccp getPngIccp(void);
	void setPngIccp(PngIccp);
	__int32 getPngSrgb(void);
	void setPngSrgb(__int32);
	PngPalette getPngPalette(void);
	void setPngPalette(PngPalette);
	png_color_16p getPngBkgd(void);
	void setPngBkgd(png_color_16p);
	png_uint_16p getPngHist(void);
	void setPngHist(png_uint_16p);
	PngOffsets getPngOffsets(void);
	void setPngOffsets(PngOffsets);
	PngPcal getPngPcal(void);
	void setPngPcal(PngPcal);
	PngPhys getPngPhys(void);
	void setPngPhys(PngPhys);
	png_color_8p getPngSigBit(void);
	void setPngSigBit(png_color_8p);
	PngScal getPngScal(void);
	void setPngScal(PngScal);
	PngText getPngText(bool);
	void setPngText(PngText, bool);
	png_timep getPngTime(bool);
	void setPngTime(png_timep, bool);
	PngTrans getPngTrans(void);
	void setPngTrans(PngTrans);
};

#endif