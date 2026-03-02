#include <merge/Layer.h>

	////////////////////////////////
	// Constructors & Destructors //
	////////////////////////////////

Layer::Layer(Header info, byte** rows, LayerLoc position) {
	this->rows = rows;
	this->info = info;
	this->position = position;
	this->bg_color = -1;
	this->flags = 0;
}

Layer::~Layer(void) {
	this->Delete();
}

	////////////////////
	// Set/Get Pixels //
	////////////////////

void Layer::set_pixel_rgb(size_t x, size_t y, size_t rgbcolor) {
	int x_ = 3 * x;
	this->rows[y][x_++] = (rgbcolor >> 16) & 0xFF; // R
	this->rows[y][x_++] = (rgbcolor >>  8) & 0xFF; // G
	this->rows[y][x_  ] = (rgbcolor      ) & 0xFF; // B
}

void Layer::set_pixel_argb(size_t x, size_t y, size_t argbcolor) {
	int x_ = 4 * x;
	this->rows[y][x_++] = (argbcolor >> 16) & 0xFF; // R
	this->rows[y][x_++] = (argbcolor >>  8) & 0xFF; // G
	this->rows[y][x_++] = (argbcolor      ) & 0xFF; // B
	this->rows[y][x_  ] = (argbcolor >> 24) & 0xFF; // A
}

void Layer::set_pixel_rgba(size_t x, size_t y, size_t rgbacolor) {
	int x_ = 4 * x;
	this->rows[y][x_++] = (rgbacolor >> 24) & 0xFF; // R
	this->rows[y][x_++] = (rgbacolor >> 16) & 0xFF; // G
	this->rows[y][x_++] = (rgbacolor >>  8) & 0xFF; // B
	this->rows[y][x_  ] = (rgbacolor      ) & 0xFF; // A
}

size_t Layer::get_pixel_argb(size_t x, size_t y) {
	size_t color = 0x0;
	y *= 4;
	color  = (unsigned)(this->rows[x][y++] << 16) & 0x00FF0000; // R
	color |= (unsigned)(this->rows[x][y++] <<  8) & 0x0000FF00; // G
	color |= (unsigned)(this->rows[x][y++]      ) & 0x000000FF; // B
	color |= (unsigned)(this->rows[x][y  ] << 24) & 0xFF000000; // A
	return color;
}

RGBA Layer::get_pixel_channels(size_t x, size_t y) {
	RGBA color;
	color.red   = this->rows[x][y++]; // R
	color.green = this->rows[x][y++]; // G
	color.blue  = this->rows[x][y++]; // B
	color.alpha = this->rows[x][y  ]; // A
	return color;
}

	////////////////////
	// Public Methods //
	////////////////////

// �������������� ���������� � ����
void Layer::ChangeHeader(Header new_header) {
	this->info = new_header;
}

// ���������� �������
// ������� ������ ������ � ������������� ��������� �� �����
void Layer::SetData(byte** new_rows) {
	this->Delete();
	this->rows = new_rows;
}

// �������� ����
void Layer::Delete(void) {
	if (this->rows) {
		for (size_t i = 0; i < this->info.height; i++)
			if (this->rows[i]) free(this->rows[i]);
		free(this->rows);
	}
}

// ����������� ��������� ���� �� ��������� �����������
// ���������� ��������� �� ���������� ������
byte** Layer::Copy(size_t x1, size_t y1, size_t x2, size_t y2) {
	size_t i, j, t, x, y, bpp = 4 * this->info.bit_depth / 8;
	byte** copy = AllocateMemory<byte>(y2 - y1, (x2 - x1) * bpp);
	for (i = y1, y = 0; i < y2; i++, y++)
		for (j = bpp * x1, x = 0, t = x1 * bpp; j < bpp * x2; j++, x++, t++)
			copy[y][x] = this->rows[i][t];
	return copy;
}

	//////////////////////////
	// Read Only Properties //
	//////////////////////////

byte** Layer::getImage(void) {
	return this->rows;
}

Header Layer::getInfo(void) {
	return this->info;
}

LayerLoc Layer::getLoc(void) {
	return this->position;
}

size_t Layer::getBgColor(void) {
	return this->bg_color;
}

byte Layer::getBgByChannel(byte channel) {
	byte result = ((this->bg_color >> (channel * 8)) & 0xFF);
	return result;
}

byte Layer::getFlags(void) {
	return this->flags;
}