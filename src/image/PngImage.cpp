#include <merge/PngImage.h>

// Constructors & Destructors

PngImage::PngImage(void) {
	this->InitVars();
}

PngImage::~PngImage(void) {
	this->Destroy();
}

// Common Methods

byte PngImage::getFormat(void) {
	return FORMAT_PNG;
}

bool PngImage::Create(FILE* file, Header header) {
	this->file = file;
	this->access_type = FILE_WRITE | FILE_BINARY;
	this->CreatePngStruct();
	this->header = header;
	png_set_write_status_fn(this->png_ptr, NULL);
	if (setjmp(png_jmpbuf(this->png_ptr))) {
		this->Destroy();
		return false;
	}
	png_init_io(this->png_ptr, this->file);
	PngIhdr ihdr = {
		header.bit_depth,
		header.color_type,
		header.interlace_type,
		header.compression_type,
		header.filter_type,
		header.width,
		header.height
	};
	this->setPngIhdr(ihdr);
	this->WriteInfo();
	this->SetInterlace();
	return true;
}

Header PngImage::Open(FILE* file, byte flags) {
	this->file = file;
	this->access_type = FILE_READ | FILE_BINARY;
	this->CreatePngStruct();
	png_set_read_status_fn(this->png_ptr, NULL);
	if (setjmp(png_jmpbuf(this->png_ptr))) {
		this->Destroy();
		this->header.row_bytes = 0;
		return this->header;
	}
	png_init_io(this->png_ptr, this->file);
	if (flags & 2) this->header.hidepth = true;
	this->ReadInfo();
	this->TransformToRGBA();
	this->SetInterlace();
	PngIhdr ihdr = this->getPngIhdr();
	this->header.bit_depth = ihdr.bit_depth;
	this->header.color_type = ihdr.color_type;
	this->header.compression_type = ihdr.compression_type;
	this->header.filter_type = ihdr.filter_type;
	this->header.interlace_type = ihdr.interlace_type;
	this->header.height = ihdr.height;
	this->header.width = ihdr.width;
	this->header.row_bytes = png_get_rowbytes(this->png_ptr, this->info_ptr);
	return this->header;
}

byte** PngImage::Read(void) {
	byte** rows = AllocateMemory<byte>(this->header.height, this->header.row_bytes);
	png_read_image(this->png_ptr, rows);
	this->ReadEndInfo();
	return rows;
}

void PngImage::Write(byte** rows) {
	png_write_image(this->png_ptr, rows);
	this->WriteEndInfo();
}

void PngImage::Destroy(void) {
	if (this->access_type == (FILE_READ | FILE_BINARY)) {
		png_destroy_read_struct(&this->png_ptr, &this->info_ptr, &this->end_info_ptr);
	} else if (this->access_type == (FILE_WRITE | FILE_BINARY)) {
		png_destroy_info_struct(this->png_ptr, &this->end_info_ptr);
		png_destroy_write_struct(&this->png_ptr, &this->info_ptr);
	}
	this->InitVars();
}

// ���������� �������� � ������� RGBA
void PngImage::TransformToRGBA(void) {
	PngIhdr ihdr = this->getPngIhdr();
	//png_set_expand_16(this->png_ptr);
	// 16-������ ������ ������ �� 8 ���
	if (ihdr.bit_depth == 16 && !this->header.hidepth)
		png_set_strip_16(this->png_ptr);
	// ������� ����������� � RGB
	if (ihdr.color_type == PNG_COLOR_TYPE_PALETTE && ihdr.bit_depth <= 8)
		png_set_palette_to_rgb(this->png_ptr);
	// ������� ������ ���� � RGB
	if (ihdr.color_type == PNG_COLOR_TYPE_GRAY ||
		ihdr.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(this->png_ptr);
	// ��������� ������ �����-�����
	if (png_get_valid(this->png_ptr, this->info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(this->png_ptr);
	} else png_set_add_alpha(this->png_ptr, 0xFF, PNG_FILLER_AFTER);
	// �������� ���������� � ����������
	png_read_update_info(this->png_ptr, this->info_ptr);
}

// RGBA <=> ARGB
void PngImage::TransformSwapAlpha(void) {
	png_set_swap_alpha(this->png_ptr);
	png_read_update_info(this->png_ptr, this->info_ptr);
}

void PngImage::InitFromFile(PngImage* png) {
	this->setPngIhdr(png->getPngIhdr());
	this->setPngChrm(png->getPngChrm());
	this->setPngGamma(png->getPngGamma());
	this->setPngIccp(png->getPngIccp());
	this->setPngSrgb(png->getPngSrgb());
	this->setPngPalette(png->getPngPalette());
	this->setPngBkgd(png->getPngBkgd());
	this->setPngHist(png->getPngHist());
	this->setPngOffsets(png->getPngOffsets());
	this->setPngPcal(png->getPngPcal());
	this->setPngPhys(png->getPngPhys());
	this->setPngSigBit(png->getPngSigBit());
	this->setPngScal(png->getPngScal());
	this->setPngText(png->getPngText(false), false);
	this->setPngTime(png->getPngTime(false), false);
	this->setPngTrans(png->getPngTrans());
}

void PngImage::EndFromFile(PngImage* png) {
	this->setPngText(png->getPngText(true), true);
	this->setPngTime(png->getPngTime(true), true);
}

void PngImage::ReadInfo(void) {
	png_read_info(this->png_ptr, this->info_ptr);
}

void PngImage::WriteInfo(void) {
	png_write_info(this->png_ptr, this->info_ptr);
}

void PngImage::ReadEndInfo(void) {
	png_read_end(this->png_ptr, this->end_info_ptr);
}

void PngImage::WriteEndInfo(void) {
	png_write_end(this->png_ptr, this->end_info_ptr);
}

void PngImage::SetInterlace(void) {
#if defined(PNG_READ_INTERLACING_SUPPORTED) || defined(PNG_WRITE_INTERLACING_SUPPORTED)
	if (this->access_type == (FILE_READ | FILE_BINARY)) {
		this->num_pass = png_set_interlace_handling(this->png_ptr);
	} else if (this->access_type == (FILE_WRITE | FILE_BINARY)) {
		png_set_interlace_handling(png_ptr);
	}
#else
	this->num_pass = 1;
#endif
}

// Private Methods

void PngImage::InitVars(void) {
	this->file = NULL;
	this->access_type = 0;
	this->header.hidepth = false;
	this->header.width = 0;
	this->header.height = 0;
	this->header.row_bytes = 0;
	this->num_pass = 1;
}

void PngImage::CreatePngStruct(void) {
	if (this->access_type == (FILE_READ | FILE_BINARY)) {
		this->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		this->info_ptr = png_create_info_struct(this->png_ptr);
		this->end_info_ptr = png_create_info_struct(this->png_ptr);
	} else if (this->access_type == (FILE_WRITE | FILE_BINARY)) {
		this->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		this->info_ptr = png_create_info_struct(this->png_ptr);
		this->end_info_ptr = png_create_info_struct(this->png_ptr);
	}
}

// PNG Properties

PngIhdr PngImage::getPngIhdr(void) {
	PngIhdr ihdr = {0,0,0,0,0,0,0};
	png_get_IHDR(this->png_ptr, this->info_ptr, &ihdr.width, &ihdr.height, &ihdr.bit_depth,
		&ihdr.color_type, &ihdr.interlace_type, &ihdr.compression_type, &ihdr.filter_type);
	return ihdr;
}

void PngImage::setPngIhdr(PngIhdr ihdr) {
	png_set_IHDR(this->png_ptr, this->info_ptr, ihdr.width, ihdr.height, ihdr.bit_depth,
		ihdr.color_type, ihdr.interlace_type, ihdr.compression_type, ihdr.filter_type);
}

PngChrm PngImage::getPngChrm(void) {
	PngChrm chrm = {0,0,0,0,0,0,0,0};
#ifdef PNG_cHRM_SUPPORTED
	png_get_cHRM_fixed(this->png_ptr, this->info_ptr, &chrm.white_x, &chrm.white_y,
		&chrm.red_x, &chrm.red_y, &chrm.green_x, &chrm.green_y, &chrm.blue_x, &chrm.blue_y);
#endif
	return chrm;
}

void PngImage::setPngChrm(PngChrm chrm) {
#ifdef PNG_cHRM_SUPPORTED
	png_set_cHRM_fixed(this->png_ptr, this->info_ptr, chrm.white_x, chrm.white_y,
		chrm.red_x, chrm.red_y, chrm.green_x, chrm.green_y, chrm.blue_x, chrm.blue_y);
#endif
}

png_fixed_point PngImage::getPngGamma(void) {
	png_fixed_point gamma = 0;
#ifdef PNG_gAMA_SUPPORTED
	png_get_gAMA_fixed(this->png_ptr, this->info_ptr, &gamma);
#endif
	return gamma;
}

void PngImage::setPngGamma(png_fixed_point gamma) {
#ifdef PNG_gAMA_SUPPORTED
	png_set_gAMA_fixed(this->png_ptr, this->info_ptr, gamma);
#endif
}

PngIccp PngImage::getPngIccp(void) {
	PngIccp iccp = {NULL,0,0,0};
#ifdef PNG_iCCP_SUPPORTED
	png_get_iCCP(this->png_ptr, this->info_ptr, &iccp.name, &iccp.compression_type, &iccp.profile, &iccp.proflen);
#endif
	return iccp;
}

void PngImage::setPngIccp(PngIccp iccp) {
#ifdef PNG_iCCP_SUPPORTED
	if (iccp.name)
		png_set_iCCP(this->png_ptr, this->info_ptr, iccp.name, iccp.compression_type, iccp.profile, iccp.proflen);
#endif
}

__int32 PngImage::getPngSrgb(void) {
	int intent = 0;
#ifdef PNG_sRGB_SUPPORTED
	png_get_sRGB(this->png_ptr, this->info_ptr, &intent);
#endif
	return intent;
}

void PngImage::setPngSrgb(__int32 intent) {
#ifdef PNG_sRGB_SUPPORTED
	png_set_sRGB(this->png_ptr, this->info_ptr, intent);
#endif
}

PngPalette PngImage::getPngPalette(void) {
	PngPalette palette = {NULL,0};
	png_get_PLTE(this->png_ptr, this->info_ptr, &palette.palette, &palette.num_palette);
	return palette;
}

void PngImage::setPngPalette(PngPalette palette) {
	if (palette.palette)
		png_set_PLTE(this->png_ptr, this->info_ptr, palette.palette, palette.num_palette);
}

png_color_16p PngImage::getPngBkgd(void) {
	png_color_16p background = NULL;
#ifdef PNG_bKGD_SUPPORTED
	png_get_bKGD(this->png_ptr, this->info_ptr, &background);
#endif
	return background;
}

void PngImage::setPngBkgd(png_color_16p background) {
#ifdef PNG_bKGD_SUPPORTED
	if (background)
		png_set_bKGD(this->png_ptr, this->info_ptr, background);
#endif
}

png_uint_16p PngImage::getPngHist(void) {
	png_uint_16p hist = NULL;
#ifdef PNG_hIST_SUPPORTED
	png_get_hIST(this->png_ptr, this->info_ptr, &hist);
#endif
	return hist;
}

void PngImage::setPngHist(png_uint_16p hist) {
#ifdef PNG_hIST_SUPPORTED
	if (hist)
		png_set_hIST(this->png_ptr, this->info_ptr, hist);
#endif
}

PngOffsets PngImage::getPngOffsets(void) {
	PngOffsets offsets = {0,0,0};
#ifdef PNG_oFFs_SUPPORTED
	png_get_oFFs(this->png_ptr, this->info_ptr, &offsets.offset_x, &offsets.offset_y, &offsets.unit_type);
#endif
	return offsets;
}

void PngImage::setPngOffsets(PngOffsets offsets) {
#ifdef PNG_oFFs_SUPPORTED
	png_set_oFFs(this->png_ptr, this->info_ptr, offsets.offset_x, offsets.offset_y, offsets.unit_type);
#endif
}

PngPcal PngImage::getPngPcal(void) {
	PngPcal pcal = {NULL,NULL,NULL,0,0,0,0};
#ifdef PNG_pCAL_SUPPORTED
	png_get_pCAL(this->png_ptr, this->info_ptr, &pcal.purpose, &pcal.X0,
		&pcal.X1, &pcal.type, &pcal.nparams, &pcal.units, &pcal.params);
#endif
	return pcal;
}

void PngImage::setPngPcal(PngPcal pcal) {
#ifdef PNG_pCAL_SUPPORTED
	if (pcal.purpose && pcal.units && pcal.params)
		png_set_pCAL(this->png_ptr, this->info_ptr, pcal.purpose, pcal.X0,
			pcal.X1, pcal.type, pcal.nparams, pcal.units, pcal.params);
#endif
}

PngPhys PngImage::getPngPhys(void) {
	PngPhys phys = {0,0,0};
#ifdef PNG_pHYs_SUPPORTED
	png_get_pHYs(this->png_ptr, this->info_ptr, &phys.res_x, &phys.res_y, &phys.unit_type);
#endif
	return phys;
}

void PngImage::setPngPhys(PngPhys phys) {
#ifdef PNG_pHYs_SUPPORTED
	png_set_pHYs(this->png_ptr, this->info_ptr, phys.res_x, phys.res_y, phys.unit_type);
#endif
}

png_color_8p PngImage::getPngSigBit(void) {
	png_color_8p sig_bit = NULL;
#ifdef PNG_sBIT_SUPPORTED
	png_get_sBIT(this->png_ptr, this->info_ptr, &sig_bit);
#endif
	return sig_bit;
}

void PngImage::setPngSigBit(png_color_8p sig_bit) {
#ifdef PNG_sBIT_SUPPORTED
	if (sig_bit)
		png_set_sBIT(this->png_ptr, this->info_ptr, sig_bit);
#endif
}

PngScal PngImage::getPngScal(void) {
	PngScal scal = {0,0.0,0.0};
#ifdef PNG_sCAL_SUPPORTED
	png_get_sCAL(this->png_ptr, this->info_ptr, &scal.unit, &scal.scal_width, &scal.scal_height);
#endif
	return scal;
}

void PngImage::setPngScal(PngScal scal) {
#ifdef PNG_sCAL_SUPPORTED
	png_set_sCAL(this->png_ptr, this->info_ptr, scal.unit, scal.scal_width, scal.scal_height);
#endif
}

PngText PngImage::getPngText(bool end) {
	PngText text = {NULL,0};
#ifdef PNG_TEXT_SUPPORTED
	if (end) png_get_text(this->png_ptr, this->end_info_ptr, &text.text_ptr, &text.num_text);
	else png_get_text(this->png_ptr, this->info_ptr, &text.text_ptr, &text.num_text);
#endif
	return text;
}

void PngImage::setPngText(PngText text, bool end) {
#ifdef PNG_TEXT_SUPPORTED
	if (text.text_ptr) {
		if (end) png_set_text(this->png_ptr, this->end_info_ptr, text.text_ptr, text.num_text);
		else png_set_text(this->png_ptr, this->info_ptr, text.text_ptr, text.num_text);
	}
#endif
}

png_timep PngImage::getPngTime(bool end) {
	png_timep mod_time = NULL;
#ifdef PNG_tIME_SUPPORTED
	if (end) png_get_tIME(this->png_ptr, this->end_info_ptr, &mod_time);
	else png_get_tIME(this->png_ptr, this->info_ptr, &mod_time);
#endif
	return mod_time;
}

void PngImage::setPngTime(png_timep mod_time, bool end) {
#ifdef PNG_tIME_SUPPORTED
	if (mod_time) {
		if (end) png_set_tIME(this->png_ptr, this->end_info_ptr, mod_time);
		else png_set_tIME(this->png_ptr, this->info_ptr, mod_time);
	}
#endif
}

PngTrans PngImage::getPngTrans(void) {
	PngTrans trans = {NULL,0,NULL};
#ifdef PNG_tRNS_SUPPORTED
	png_get_tRNS(this->png_ptr, this->info_ptr, &trans.trans_alpha, &trans.num_trans, &trans.trans_color);
#endif
	return trans;
}

void PngImage::setPngTrans(PngTrans trans) {
#ifdef PNG_tRNS_SUPPORTED
	__int32 sample_max = (1 << this->header.bit_depth);
	if (trans.trans_alpha && trans.trans_color &&
		!((this->header.color_type == PNG_COLOR_TYPE_GRAY &&
		(__int32)trans.trans_color->gray > sample_max) ||
		(this->header.color_type == PNG_COLOR_TYPE_RGB &&
		((__int32)trans.trans_color->red > sample_max ||
		(__int32)trans.trans_color->green > sample_max ||
		(__int32)trans.trans_color->blue > sample_max))))
		png_set_tRNS(this->png_ptr, this->info_ptr, trans.trans_alpha, trans.num_trans, trans.trans_color);
#endif
}