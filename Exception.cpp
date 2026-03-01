#include "Exception.h"

Exception::Exception(void) {
	this->code = 0;
	this->message = "Unknown Exception";
}

Exception::Exception(size_t code) {
	this->code = code;
	this->message = "Exception";
}

Exception::Exception(size_t code, char* message) {
	this->code = code;
	this->message = (char*)malloc(strlen(message) * sizeof(char));
	if (this->message == NULL) this->message = "Exception";
	else strcpy(this->message, message);
}

Exception::Exception(size_t code, char* msg1, char* msg2) {
	this->code = code;
	this->message = (char*)malloc((strlen(msg1) + strlen(msg2) + 1) * sizeof(char));
	if (this->message == NULL) this->message = "Exception";
	else sprintf(this->message, "%s%s", msg1, msg2);
}

Exception::~Exception(void) {}

size_t Exception::getCode(void) {
	return this->code;
}

char* Exception::getMessage(void) {
	return this->message;
}