#include <merge/FileImage.h>

FileImage::FileImage(void) {
	this->access = 0;
	this->file = FILE_NULL;
	this->out = NULL;
}

FileImage::~FileImage(void) {
	this->Close();
	this->Destroy();
}

bool FileImage::Create(char* filename, Header header) {
	this->file = fopen(filename, "wb");
	if (this->file == NULL)
		throw Exception(ERROR_FOPEN, "Can't open file for binary write: ", filename);
	this->info = header;
	this->out = new PngImage();
	return this->out->Create(this->file, this->info);
}

Header FileImage::Open(char* filename, byte flags) {
	this->Destroy();
	this->file = fopen(filename, "rb");
	if (this->file == NULL)
		throw Exception(ERROR_FOPEN, "Can't open file for binary read: ", filename);
	byte sig[8] = {0};
	fread(sig, 1, 8, this->file);
	rewind(this->file);
	if (png_check_sig(sig, 8)) {
		this->out = new PngImage();
	} else {
		fclose(this->file);
	}
	this->info = this->out->Open(this->file, flags);
	return this->info;
}

byte** FileImage::Read(void) {
	try {
		return this->out->Read();
	} catch(Exception e) { throw e; }
}

void FileImage::Write(byte** rows) {
	this->out->Write(rows);
}

void FileImage::Close(void) {
	if (this->file)
		fclose(this->file);
	this->file = NULL;
}

void FileImage::Destroy(void) {
	if (this->out)
		this->out->Destroy();
	this->out = NULL;
}