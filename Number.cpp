#include "Number.h"

// Constructor
Number::Number(byte type, Image* source) {
	this->type = type;
	this->source = source;
	this->number = NULL;
	this->triple = false;
}

// Destructor
Number::~Number(void) {
	this->dest = NULL;
	this->source = NULL;
	this->number = NULL;
}

// Установка типа номера
Number* Number::setType(byte type) {
	this->type = type;
	return this;
}

// Генерация номера из строки
Image* Number::Generate(char* number_string) {
	this->number = number_string;
	this->dest = new Image();
	switch(this->type) {
		case NUM_TYPE_RU:
			return this->GenerateRu();
		case NUM_TYPE_RUT:
			return this->GenerateRu();
		case NUM_TYPE_EN:
			return this->GenerateEn();
		case NUM_TYPE_ENT:
			return this->GenerateEn();
		default:
			throw new Exception(0x0, "Bad number type");
	}
}

// Индексы букв для российских номеров
byte Number::getIndexRu(char letter) {
	char let = (char)tolower(letter);
	for (byte i = 0; i < NUM_RU_COUNT; i++)
		if (num_ru_index[i] == let) return i;
	return -1;
}

// Индексы букв для европейских номеров
byte Number::getIndexEn(char letter) {
	char let = (char)tolower(letter);
	for (byte i = 0; i < NUM_EN_COUNT; i++)
		if (num_en_index[i] == let) return i;
	return -1;
}

// Generate new RU number
Image* Number::GenerateRu(void) {
	size_t x1, x2, y1, y2, t1, t2, t3, dx, dy, i, j, index;
	i = strlen(this->number);
	if (i == 8) {
		this->triple = false;
	} else if (i == 9) {
		this->triple = true;
	} else throw new Exception(0x0, "Bad number sequence");
	// Background
	x1 = num_config[this->type][NUM_BG_START_X];
	x2 = num_config[this->type][NUM_BG_OVER_X];
	y1 = num_config[this->type][NUM_BG_START_Y];
	y2 = num_config[this->type][NUM_BG_OVER_Y];
	Header header = this->dest->GenerateHeader(x2 - x1, y2 - y1, 8);
	this->dest->Create(header);
	this->dest->AddBackground(this->source->GetBackground()->Copy(x1, y1, x2, y2), 0x0, 0x0, header);
	// Numbers
	for (i = 1; i <= 3; i++) {
		j = (this->type == NUM_TYPE_RU) ? i : i + 1;
		index = this->number[j] - 48;
		if (index < 0 || index > 9) throw new Exception(0x0, "Bad letter for number");
		t1 = num_config[this->type][NUM_MD_START_X];
		t2 = num_config[this->type][NUM_MD_PADDING];
		t3 = num_config[this->type][NUM_MD_WIDTH];
		x1 = t1 + t3 * index + t2 * index;
		x2 = x1 + t3;
		y1 = num_config[this->type][NUM_MD_START_Y];
		y2 = num_config[this->type][NUM_MD_OVER_Y];
		if (i == 1) {
			dx = num_config[this->type][NUM_D1_X];
			dy = num_config[this->type][NUM_D1_Y];
		} else if (i == 2) {
			dx = num_config[this->type][NUM_D2_X];
			dy = num_config[this->type][NUM_D2_Y];
		} else if (i == 3) {
			dx = num_config[this->type][NUM_D3_X];
			dy = num_config[this->type][NUM_D3_Y];
		}
		header = this->dest->GenerateHeader(x2 - x1, y2 - y1, 8);
		this->dest->AddLayer(this->source->GetBackground()->Copy(x1, y1, x2, y2), header, dx, dy, 1, 0x0, 0x0);
	}
	// Letters
	for (i = 1; i <= 3; i++) {
		if (i == 1) {
			j = 0;
			dx = num_config[this->type][NUM_L1_X];
			dy = num_config[this->type][NUM_L1_Y];
		} else if (i == 2) {
			j = (this->type == NUM_TYPE_RU) ? 4 : 1;
			dx = num_config[this->type][NUM_L2_X];
			dy = num_config[this->type][NUM_L2_Y];
		} else if (i == 3) {
			j = 5;
			dx = num_config[this->type][NUM_L3_X];
			dy = num_config[this->type][NUM_L3_Y];
		}
		index = this->getIndexRu(this->number[j]);
		if (index < 0) throw new Exception(0x0, "Bad letter for number");
		t1 = num_config[this->type][NUM_ML_START_X];
		t2 = num_config[this->type][NUM_ML_PADDING];
		t3 = num_config[this->type][NUM_ML_WIDTH];
		x1 = t1 + t3 * index + t2 * index;
		x2 = x1 + t3;
		y1 = num_config[this->type][NUM_ML_START_Y];
		y2 = num_config[this->type][NUM_ML_OVER_Y];
		header = this->dest->GenerateHeader(x2 - x1, y2 - y1, 8);
		this->dest->AddLayer(this->source->GetBackground()->Copy(x1, y1, x2, y2), header, dx, dy, 1, 0x0, 0x0);
	}
	// Region
	j = (this->triple) ? 8 : 7;
	for (i = 6; i <= j; i++) {
		index = this->number[i] - 48;
		if (index < 0 || index > 9) throw new Exception(0x0, "Bad letter for number");
		t1 = num_config[this->type][NUM_SD_START_X];
		t2 = num_config[this->type][NUM_SD_PADDING];
		t3 = num_config[this->type][NUM_SD_WIDTH];
		x1 = t1 + t3 * index + t2 * index;
		x2 = x1 + t3;
		y1 = num_config[this->type][NUM_SD_START_Y];
		y2 = num_config[this->type][NUM_SD_OVER_Y];
		if (this->triple) {
			if (i == 6) {
				dx = num_config[this->type][NUM_R3_D1_X];
				dy = num_config[this->type][NUM_R3_D1_Y];
			} else if (i == 7) {
				dx = num_config[this->type][NUM_R3_D2_X];
				dy = num_config[this->type][NUM_R3_D2_Y];
			} else if (i == 8) {
				dx = num_config[this->type][NUM_R3_D3_X];
				dy = num_config[this->type][NUM_R3_D3_Y];
			}
		} else {
			if (i == 6) {
				dx = num_config[this->type][NUM_R2_D1_X];
				dy = num_config[this->type][NUM_R2_D1_Y];
			} else if (i == 7) {
				dx = num_config[this->type][NUM_R2_D2_X];
				dy = num_config[this->type][NUM_R2_D2_Y];
			}
		}
		header = this->dest->GenerateHeader(x2 - x1, y2 - y1, 8);
		this->dest->AddLayer(this->source->GetBackground()->Copy(x1, y1, x2, y2), header, dx, dy, 1, 0x0, 0x0);
	}
	// Return
	return this->dest->Flatten();
}

// Generate new EN number
Image* Number::GenerateEn(void) {
	size_t x1, x2, y1, y2, t1, t2, t3, dx, dy, i, j, index;
	i = strlen(this->number);
	if (i != 7) throw new Exception(0x0, "Bad number sequence");
	// Background
	x1 = num_config[this->type][NUM_BG_START_X];
	x2 = num_config[this->type][NUM_BG_OVER_X];
	y1 = num_config[this->type][NUM_BG_START_Y];
	y2 = num_config[this->type][NUM_BG_OVER_Y];
	Header header = this->dest->GenerateHeader(x2 - x1, y2 - y1, 8);
	this->dest->Create(header);
	this->dest->AddBackground(this->source->GetBackground()->Copy(x1, y1, x2, y2), 0x0, 0x0, header);
	// Letters
	for (i = 0; i <= 3; i++) {
		if (i == 0) {
			dx = num_config[this->type][NUM_L1_X];
			dy = num_config[this->type][NUM_L1_Y];
		} else if (i == 1) {
			dx = num_config[this->type][NUM_L2_X];
			dy = num_config[this->type][NUM_L2_Y];
		} else if (i == 2) {
			dx = num_config[this->type][NUM_L3_X];
			dy = num_config[this->type][NUM_L3_Y];
		} else if (i == 3) {
			dx = num_config[this->type][NUM_D1_X];
			dy = num_config[this->type][NUM_D1_Y];
		}
		index = this->getIndexEn(this->number[i]);
		if (index < 0) throw new Exception(0x0, "Bad letter for number");
		t1 = num_config[this->type][NUM_ML_START_X];
		t2 = num_config[this->type][NUM_ML_PADDING];
		t3 = num_config[this->type][NUM_ML_WIDTH];
		x1 = t1 + t3 * index + t2 * index;
		x2 = x1 + t3;
		y1 = num_config[this->type][NUM_ML_START_Y];
		y2 = num_config[this->type][NUM_ML_OVER_Y];
		header = this->dest->GenerateHeader(x2 - x1, y2 - y1, 8);
		this->dest->AddLayer(this->source->GetBackground()->Copy(x1, y1, x2, y2), header, dx, dy, 1, 0x0, 0x0);
	}
	// Numbers
	for (i = 4; i <= 6; i++) {
		index = this->number[i] - 48;
		if (index < 0 || index > 9) throw new Exception(0x0, "Bad letter for number");
		t1 = num_config[this->type][NUM_MD_START_X];
		t2 = num_config[this->type][NUM_MD_PADDING];
		t3 = num_config[this->type][NUM_MD_WIDTH];
		x1 = t1 + t3 * index + t2 * index;
		x2 = x1 + t3;
		y1 = num_config[this->type][NUM_MD_START_Y];
		y2 = num_config[this->type][NUM_MD_OVER_Y];
		if (i == 4) {
			dx = num_config[this->type][NUM_D2_X];
			dy = num_config[this->type][NUM_D2_Y];
		} else if (i == 5) {
			dx = num_config[this->type][NUM_D3_X];
			dy = num_config[this->type][NUM_D3_Y];
		} else if (i == 6) {
			dx = num_config[this->type][NUM_R2_D1_X];
			dy = num_config[this->type][NUM_R2_D1_Y];
		}
		header = this->dest->GenerateHeader(x2 - x1, y2 - y1, 8);
		this->dest->AddLayer(this->source->GetBackground()->Copy(x1, y1, x2, y2), header, dx, dy, 1, 0x0, 0x0);
	}
	// Return
	return this->dest->Flatten();
}