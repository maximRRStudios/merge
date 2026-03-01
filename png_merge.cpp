#define _POSIX_SOURCE 1

#include "png_merge.h"

#ifdef WIN32
#include <conio.h>
static char* car_bkg = "D:/!_work/png_merge/race/DDR_SP4_static1.png";
static char* car_lr1 = "D:/!_work/png_merge/race/DDR_SP4_map_grey.png";
static char* car_lr2 = "D:/!_work/png_merge/race/DDR_SP4_source.png";
static char* car_lr3 = "D:/!_work/png_merge/race/DDR_SP4_color_red.png";
static char* car_out = "D:/!_work/png_merge/race/DDR_SP4_out.png";
static char* car_num_in = "D:/!_work/png_merge/race/_numbers_ru.png";
static char* car_num_in2 = "D:/!_work/png_merge/race/_numbers_en.png";
static char* car_num_out = "D:/!_work/png_merge/race/_out.png";
static char* car_num_out2 = "D:/!_work/png_merge/race/_out2.png";
static char* car_num_out3 = "D:/!_work/png_merge/race/_out3.png";
static char* car_num_out4 = "D:/!_work/png_merge/race/_out4.png";
static char* img = "D:/!_work/png_merge/race/badboy2.png";
#else
static char* car_bkg = "/root/png_merge/race/DDR_SP4_static.png";
static char* car_lr1 = "/root/png_merge/race/DDR_SP4_map_grey.png";
static char* car_lr2 = "/root/png_merge/race/DDR_SP4_source.png";
static char* car_lr3 = "/root/png_merge/race/DDR_SP4_color_red200.png";
static char* car_out = "/root/png_merge/race/DDR_SP4_out.png";
static char* img = "/root/png_merge/race/badboy2.png";
#endif

int test(void) {
	Image* image = new Image();
	/*Header header = image->GenerateHeader(WIDTH, HEIGHT, BITDEPTH);
	image->AddBackground(car_lr2, FROM_IMAGE, 0x0);
	byte** rows = image->GenerateMap(0x777777); // 0x2579E3 0x7A7A7A
	Image* out = new Image();
	out->AddBackground(rows, 0, 0x0, header);
	out->Save(car_out);
	delete out;
	delete image;
	return 1;*/
	/*image->AddBackground(test_2, FROM_IMAGE, 0x0);
	image->AddLayer(test_1, 0, 0, 1, COPY_ALPHA, 0x0);
	image->Flatten();
	image->Save(car_out);
	return 1;*/
	/*image->AddLayer(car_lr2, 0, 0, 0, FROM_IMAGE, 0x0);	// car_lr3
	Filters::Resize(image->GetLayer(0), 200, BILINEAR);
	Filters::Rotate(image->GetLayer(0), 20);
	image->Save(car_out);
	delete image;
	return 0;*/
	//image->AddLayer(car_tmp0, 0, 0, 0, FROM_IMAGE, 0x0);
	image->AddBackground(car_num_in, FROM_IMAGE, 0x0);
	Number* number = new Number(NUM_TYPE_RUT, image);
	number->Generate("CE023T74")->Save(car_num_out);
	number->setType(NUM_TYPE_RU)->Generate("C023YT74")->Save(car_num_out2);
	Image* img_en = new Image();
	img_en->AddBackground(car_num_in2, FROM_IMAGE, 0x0);
	Number* num_en = new Number(NUM_TYPE_EN, img_en);
	num_en->Generate("ABCD123")->Save(car_num_out3);
	num_en->setType(NUM_TYPE_ENT)->Generate("DCBA321")->Save(car_num_out4);
	/*image->AddLayer(car_lr3, 0, 0, 0, FROM_IMAGE, 0x0);	// car_lr3
	image->AddLayer(car_lr2, 0, 0, 101, COLOR_MAP | COPY_ALPHA | CROP_BACKGROUND, 0x0);	// car_lr2
	image->AddLayer(img, 140, 50, 1, 0, 0x0);	// img
	image->AddLayer(car_bkg, 0, 0, 102, 0, 0x0); // car_bkg*/
	//image->Flatten();
	//image->Save(car_out);
	delete image;
#ifdef WIN32
	//getch();
#endif
	return 0;
}

int main_test(int argc, char *argv[]) {
	if (argc < 2) return 0;
	srand((unsigned)time(NULL));
	__int32 i;
	char path[512];
	char path_color[512];
	char path_out[512];
	char path_source[512];
	char path_static[512];
	char* pPath = path;
	char* pPath_color = path_color;
	char* pPath_out = path_out;
	char* pPath_source = path_source;
	char* pPath_static = path_static;
	strcpy(pPath, argv[0]);
	for (i = strlen(pPath) - 1; i > 0; i--) {
		if (path[i] == '\\') {
			path[i + 1] = '\x0';
			break;
		}
	}
	for (i = 1; i < argc; i++) {
		sprintf(pPath_color, "%scolors\\%s", pPath, argv[i]);
		sprintf(pPath_out, "%s%s", pPath, argv[i]);
		sprintf(pPath_source, "%s.source.png", pPath, argv[i]);
		sprintf(pPath_static, "%s.static.png", pPath, argv[i]);
		Image* image = new Image();
		image->AddLayer(pPath_color, 0, 0, 0, FROM_IMAGE, 0x0);
		image->AddLayer(pPath_source, 0, 0, 1, COLOR_MAP | COPY_ALPHA | CROP_BACKGROUND, 0x0);
		image->AddLayer(pPath_static, 0, 0, 2, 0, 0x0);
		image->Flatten();
		image->Save(pPath_out);
		delete image;
	}
	return 0;
}