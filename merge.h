#ifndef _MERGE_H_828D7C7B_64BF_4ffe_BACA_1D8E67C36E18_
#define _MERGE_H_828D7C7B_64BF_4ffe_BACA_1D8E67C36E18_

#define APP_VERSION "0.4a"
#define APP_NAME "ImageMergeCGI"

#ifdef WIN32
const char* ROOT = "E:/!_work/!_CPP/png_merge/race/";
#else
const char* ROOT = "/mnt/studio/alf/race/img/";
#endif

#include <stdio.h>
#include "Number.h"
#include "Image.h"
#include <sys/stat.h>
#if defined(WIN32) && defined (_DEBUG)
#include "png_merge.h"
#include <conio.h>
#endif

//int main(int, char**);
void copy(char*, char*);
void delete_file(char*);
void makedirs(char*);

#endif