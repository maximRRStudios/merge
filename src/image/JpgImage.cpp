#include <merge/JpgImage.h>

JpgImage::JpgImage(void) {
	this->InitVars();
}

JpgImage::~JpgImage(void) {
	this->Destroy();
}

// Common Methods

byte JpgImage::getFormat(void) {
	return FORMAT_JPG;
}

bool JpgImage::Create(FILE* file, Header header) {
	return true;
}

Header JpgImage::Open(FILE* file) {
	Header header = {0,0,0,0,0,0,0,0};
	return header;
}

byte** JpgImage::Read(void) {
	return NULL;
}

void JpgImage::Write(byte** rows) {}

void JpgImage::Destroy(void) {}

// Private Methods

void JpgImage::InitVars(void) {
	this->file = NULL;
	this->access_type = 0;
	this->header.width = 0;
	this->header.height = 0;
	this->header.row_bytes = 0;
}