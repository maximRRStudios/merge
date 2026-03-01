#define _POSIX_SOURCE 1

#include "merge.h"

// layer=
//		<FileName>
//		<X>
//		<Y>
//		<Z>
//		<Filters>
//		<Color>
//		<Scale>
//		<Angle>
// out=
//		<FileName>
// copy=
//		<SourceFileName>
//		<DestFileName>
int main(int argc, char** argv) {
#ifdef _DEBUG
	//return test();
	char* query = "layer=DDR_SP4_color_red.png|0|0|0|1|0|0|0&layer=DDR_SP4_source.png|0|0|2|42|0|0|0&layer=DDR_SP4_static1.png|0|0|3|0|0|0|0&layer=badboy2.png|140|52|1|4|16777215|100|0&out=DDR_SP4_out.png";
	//query = "layer=DDR_SP4_color_red.png|0|0|0|1|0|0|0&layer=MazdaSpeed6_source.png|0|0|2|42|0|0|0&layer=MazdaSpeed6_static.png|0|0|3|0|0|0|0&out=MazdaSpeed6_out.png";
	//query = "copy=DDR_SP4_source.png|DDR_SP4_out_copy.png";
	//query = "layer=DDR_SP4_source.png|0|0|0|5|0|100|0&out=asdf.png";
	//query = "source=_numbers_ru.png&type=0&number=x123yt74&out=__out.png";
	//query = getenv("QUERY_STRING");
#else
	char* query = getenv("QUERY_STRING");
#endif
	printf("Content-type: text/plain\n\n");
	srand((unsigned)time(NULL));
	const byte MAX_ARGS = 30;
	const byte MAX_PARAMS = 7;
	const size_t BUF_SIZE = 512;
	const size_t STRINGS_BUF = 2048;
	char buf[MAX_ARGS][BUF_SIZE];
	if (query == NULL || strlen(query) < 1) {
		printf("{\"Version\":%s}", APP_VERSION);
		return 0x0; }
	size_t i, j, k, len = strlen(query);
	memset(buf, 0x0, MAX_ARGS * BUF_SIZE * sizeof(char));
	for (i = 0, j = 0, k = 0; i < len; i++) {
		if (query[i] == '&') {
			buf[j][k] = 0;
			if (j + 1 >= MAX_ARGS) break;
			else j++;
			k = 0;
		} else buf[j][k++] = query[i];
	} buf[j][k] = 0;
	byte index, type = 255, errors = 0, warnings = 0;
	size_t args[MAX_PARAMS];
	char path[BUF_SIZE];
	char cp_in[BUF_SIZE];
	char cp_out[BUF_SIZE];
	char out[BUF_SIZE];
	char errors_buf[STRINGS_BUF];
	char warnings_buf[STRINGS_BUF];
	char* pErrors = errors_buf;
	char* pWarnings = warnings_buf;
	char* pPath = path;
	char* pCIn = cp_in;
	char* pCOut = cp_out;
	char* pOut = out;
	char* token = NULL;
	struct stat status;
	memset(args, 0x0, MAX_PARAMS * sizeof(size_t));
	memset(errors_buf, 0x0, STRINGS_BUF * sizeof(byte));
	memset(warnings_buf, 0x0, STRINGS_BUF * sizeof(byte));
	Image* image = new Image();
	for (i = 0; i <= j; i++) {
		token = strtok(buf[i], "=");
		if (strcmp(token, "layer") == 0) {
			token = strtok(NULL, "=");
			if (!token) {
				if (strlen(warnings_buf) + 50 < STRINGS_BUF) {
					if (warnings) pWarnings += sprintf(pWarnings, ",");
					pWarnings += sprintf(pWarnings, "{\"code\":0,\"message\":\"Bad layer parameters\"}");
					warnings++;
				}
				continue;
			}
//printf("Processing %s\n", token);
			token = strtok(token, "|");
			sprintf(pPath, "%s%s", ROOT, token);
			for (k = 0; k < MAX_PARAMS; k++) {
				token = strtok(NULL, "|");
				if (!token) {
					if (strlen(warnings_buf) + 50 < STRINGS_BUF) {
						if (warnings) pWarnings += sprintf(pWarnings, ",");
						pWarnings += sprintf(pWarnings, "{\"code\":1,\"message\":\"Too few layer parameters\"}");
						warnings++;
					}
					goto SKIP_ADDLAYER;
				}
				args[k] = atoi(token);
			}
			try {
				index = image->AddLayer(path, args[0], args[1], args[2], args[3], args[4]);
//printf("Layer index: %d\n", index);
				if (args[5] > 0 && args[5] != 100) Filters::Resize(image->GetLayer(index), args[5], BILINEAR);
			} catch (Exception e) {
				len = strlen(e.getMessage());
				if (strlen(errors_buf) + len < STRINGS_BUF) {
					if (errors) pErrors += sprintf(pErrors, ",");
					pErrors += sprintf(pErrors, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
					errors++;
				}
			}
			SKIP_ADDLAYER:;
		} else if (strcmp(token, "out") == 0) {
			token =  strtok(NULL, "=");
			if (!token) {
				if (strlen(warnings_buf) + 50 < STRINGS_BUF) {
					if (warnings) pWarnings += sprintf(pWarnings, ",");
					pWarnings += sprintf(pWarnings, "{\"code\":2,\"message\":\"Output file name not set\"}");
					warnings++;
				}
				continue;
			}
			sprintf(pOut, "%s%s", ROOT, token);
			makedirs(pOut);
			/*sprintf(pOut, "%s%d%d.png", out_path, timestamp(), rand());
			while (stat(pOut, &status) == 0)
				sprintf(pOut, "%s%d%d%d.png", out_path, timestamp(), rand(), rand());*/
		} else if (strcmp(token, "number") == 0) {
			token =  strtok(NULL, "=");
			if (!token) {
				if (strlen(errors_buf) + 50 < STRINGS_BUF) {
					if (errors) pErrors += sprintf(pErrors, ",");
					pErrors += sprintf(pErrors, "{\"code\":3,\"message\":\"Bad parameters\"}");
					errors++;
				}
				continue;
			}
			sprintf(pCIn, "%s", token);
		} else if (strcmp(token, "source") == 0) {
			token =  strtok(NULL, "=");
			if (!token) {
				if (strlen(errors_buf) + 50 < STRINGS_BUF) {
					if (errors) pErrors += sprintf(pErrors, ",");
					pErrors += sprintf(pErrors, "{\"code\":3,\"message\":\"Bad parameters\"}");
					errors++;
				}
				continue;
			}
			sprintf(pCOut, "%s%s", ROOT, token);
		} else if (strcmp(token, "type") == 0) {
			token =  strtok(NULL, "=");
			if (!token) {
				if (strlen(errors_buf) + 50 < STRINGS_BUF) {
					if (errors) pErrors += sprintf(pErrors, ",");
					pErrors += sprintf(pErrors, "{\"code\":3,\"message\":\"Bad parameters\"}");
					errors++;
				}
				continue;
			}
			type = (byte)atoi(token);
		} else if (strcmp(token, "exists") == 0) {
			token =  strtok(NULL, "=");
			if (!token) {
				if (strlen(errors_buf) + 50 < STRINGS_BUF) {
					if (errors) pErrors += sprintf(pErrors, ",");
					pErrors += sprintf(pErrors, "{\"code\":3,\"message\":\"Bad parameters\"}");
					errors++;
				}
				continue;
			}
			sprintf(pCIn, "%s%s", ROOT, token);
			if (stat(pCIn, &status) != 0) {
				if (strlen(errors_buf) + 50 < STRINGS_BUF) {
					if (errors) pErrors += sprintf(pErrors, ",");
					pErrors += sprintf(pErrors, "{\"code\":10,\"message\":\"File not found\"}");
					errors++;
				}
			}
		} else if (strcmp(token, "copy") == 0) {
			token =  strtok(NULL, "=");
			if (!token) {
				if (strlen(warnings_buf) + 50 < STRINGS_BUF) {
					if (warnings) pWarnings += sprintf(pWarnings, ",");
					pWarnings += sprintf(pWarnings, "{\"code\":3,\"message\":\"Bad copy parameters\"}");
					warnings++;
				}
				continue;
			}
			token = strtok(token, "|");
			sprintf(pCIn, "%s%s", ROOT, token);
			token = strtok(NULL, "|");
			if (!token) {
				if (strlen(warnings_buf) + 50 < STRINGS_BUF) {
					if (warnings) pWarnings += sprintf(pWarnings, ",");
					pWarnings += sprintf(pWarnings, "{\"code\":4,\"message\":\"Too few copy parameters\"}");
					warnings++;
				}
				continue;
			}
			sprintf(pCOut, "%s%s", ROOT, token);
			try {
				copy(pCIn, pCOut);
			} catch (Exception e) {
				len = strlen(e.getMessage());
				if (strlen(errors_buf) + len < STRINGS_BUF) {
					if (errors) pErrors += sprintf(pErrors, ",");
					pErrors += sprintf(pErrors, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
					errors++;
				}
			}
		} else if (strcmp(token, "delete") == 0) {
			token =  strtok(NULL, "=");
			if (!token) {
				if (strlen(warnings_buf) + 50 < STRINGS_BUF) {
					if (warnings) pWarnings += sprintf(pWarnings, ",");
					pWarnings += sprintf(pWarnings, "{\"code\":5,\"message\":\"Bad delete parameters\"}");
					warnings++;
				}
				continue;
			}
			sprintf(pPath, "%s%s", ROOT, token);
			try {
				delete_file(pPath);
			} catch (Exception e) {
				len = strlen(e.getMessage());
				if (strlen(warnings_buf) + len < STRINGS_BUF) {
					if (warnings) pWarnings += sprintf(pWarnings, ",");
					pWarnings += sprintf(pWarnings, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
					warnings++;
				}
			}
		} else continue;
	}
	if (type < 255) {
		image->Clear();
		image->AddBackground(pCOut, FROM_IMAGE, 0x0);
		Number* number = new Number(type, image);
		number->Generate(pCIn)->Save(pOut);
		image->Clear();
	}
	if (image->IsImageExists()) {
		image->Flatten();
		try {
			image->Save(pOut);
		} catch (Exception e) {
			len = strlen(e.getMessage());
			if (strlen(errors_buf) + len < STRINGS_BUF) {
				if (errors) pErrors += sprintf(pErrors, ",");
				pErrors += sprintf(pErrors, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
				errors++;
			}
		}
	}
	delete image;
	pErrors = errors_buf;
	pWarnings = warnings_buf;
	if (errors > 0) printf("{\"status\":\"ERROR\",\"errors\":[%s]", pErrors);
	else printf("{\"status\":\"OK\"");
	if (warnings > 0) printf(",\"warnings\":[%s]}", pWarnings);
	else printf("}");
	//getch();
	return 0x0;
}

void makedirs(char* path) {
	int i;
	for (i = strlen(path) - 1; i > 0; i--)
		if (path[i] == '/') break;
	char buf[512];
	char* pPath = buf;
	strncpy(pPath, path, i);
	pPath[i] = 0;
	char buf2[512];
	char* pCmd = buf2;
	sprintf(pCmd, "mkdir -p %s", pPath);
	system(pCmd);
}

void copy(char* in, char* out) {
	makedirs(out);
	FILE* fin = fopen(in, "rb");
	if (fin == NULL)
		throw Exception(ERROR_FOPEN, "Could not open file for binary read: ", in);
	FILE* fout = fopen(out, "wb");
	if (fout == NULL) {
		fclose(fin);
		throw Exception(ERROR_FOPEN, "Could not open file for binary write: ", out);
	}
	const size_t BUF_SIZE = 1024;
	byte buffer[BUF_SIZE];
	size_t readed;
	while(readed = fread(buffer, sizeof(byte), BUF_SIZE, fin))
		fwrite(buffer, sizeof(byte), readed, fout);
}

void delete_file(char* filename) {
	if (remove(filename) == -1)
		throw Exception(0x101, "Could not delete ", filename);
}