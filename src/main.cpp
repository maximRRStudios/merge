#define _POSIX_SOURCE 1

#include "merge.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <direct.h>

// Вспомогательная: безопасное создание директорий
void makedirs(const char* filepath) {
    char temp[512];
    size_t len;

    snprintf(temp, sizeof(temp), "%s", filepath);
    len = strlen(temp);
    if (len == 0) return;
    if (temp[len - 1] == '/') temp[len - 1] = '\0';

    for (char* p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
#ifdef _WIN32
            _mkdir(temp);  // или mkdir(temp)
#else
            mkdir(temp, 0755);
#endif
            *p = '/';
        }
    }
#ifdef _WIN32
    _mkdir(temp);
#else
    mkdir(temp, 0755);
#endif
}

// Проверка: безопасное имя файла (без .., / в начале и т.п.)
int is_safe_filename(const char* name) {
    if (!name || strlen(name) == 0) return 0;
    if (strstr(name, "..") != NULL) return 0;
    if (name[0] == '/') return 0;
    for (const char* p = name; *p; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '-') return 0;
    }
    const char* ext = strrchr(name, '.');
    if (!ext) return 0;
    if (strcmp(ext, ".png") != 0 && strcmp(ext, ".jpg") != 0 && strcmp(ext, ".jpeg") != 0) return 0;
    return 1;
}

// Генерация безопасного выходного пути
int make_output_path(const char* filename, char* out_buf, size_t buf_size) {
    if (!is_safe_filename(filename)) {
        return 0;  // Небезопасное имя
    }
    snprintf(out_buf, buf_size, "%s%s", ROOT, filename);
    // Убедимся, что путь остаётся внутри ROOT
    if (strncmp(out_buf, ROOT, strlen(ROOT)) != 0) {
        return 0;
    }
    return 1;
}

// Безопасное копирование
void copy(char* in, char* out) {
    // Проверяем входной файл
    if (strncmp(in, ROOT, strlen(ROOT)) != 0)
        throw Exception(ERROR_ACCESS, "Access denied: input path outside ROOT", in);

    makedirs(out);

    FILE* fin = fopen(in, "rb");
    if (!fin)
        throw Exception(ERROR_FOPEN, "Could not open file for read: ", in);

    FILE* fout = fopen(out, "wb");
    if (!fout) {
        fclose(fin);
        throw Exception(ERROR_FOPEN, "Could not open file for write: ", out);
    }

    unsigned char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
        fwrite(buffer, 1, n, fout);
    }

    fclose(fin);
    fclose(fout);
}

// Безопасное удаление
void delete_file(char* filename) {
    char fullpath[512];
    if (!make_output_path(filename, fullpath, sizeof(fullpath)))
        throw Exception(0x101, "Invalid file path for deletion", filename);

    if (remove(fullpath) == -1)
        throw Exception(0x101, "Could not delete file: ", filename);
}

int main(int argc, char** argv) {
#ifdef _DEBUG
    char* query = "layer=DDR_SP4_color_red.png|0|0|0|1|0|0|0&layer=DDR_SP4_source.png|0|0|2|42|0|0|0&layer=DDR_SP4_static1.png|0|0|3|0|0|0|0&out=DDR_SP4_out.png";
#else
    char* query = getenv("QUERY_STRING");
#endif

    printf("Content-type: text/plain\n\n");
    srand((unsigned)time(NULL));

    const size_t MAX_ARGS = 30;
    const size_t MAX_PARAMS = 7;
    const size_t BUF_SIZE = 512;
    const size_t STRINGS_BUF = 2048;

    char buf[MAX_ARGS][BUF_SIZE];
    size_t i, j, k, len;

    if (query == NULL || (len = strlen(query)) == 0) {
        printf("{\"Version\":\"%s\"}\n", APP_VERSION);
        return 0;
    }

    memset(buf, 0, sizeof(buf));
    for (i = 0, j = 0, k = 0; i < len; i++) {
        if (query[i] == '&') {
            buf[j][k] = '\0';
            if (++j >= MAX_ARGS) break;
            k = 0;
        } else if (k < BUF_SIZE - 1) {
            buf[j][k++] = query[i];
        }
    }
    buf[j][k] = '\0';

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

    struct stat status;
    memset(args, 0, sizeof(args));
    memset(errors_buf, 0, STRINGS_BUF);
    memset(warnings_buf, 0, STRINGS_BUF);

    Image* image = NULL;
    try {
        image = new Image();
    } catch (...) {
        printf("{\"status\":\"ERROR\",\"errors\":[{\"code\":99,\"message\":\"Failed to create image context\"}]}");
        return 1;
    }

    for (i = 0; i <= j; i++) {
        char* token = strtok(buf[i], "=");
        if (!token) continue;

        if (strcmp(token, "layer") == 0) {
            token = strtok(NULL, "=");
            if (!token) {
                if (strlen(warnings_buf) + 60 < STRINGS_BUF) {
                    if (warnings++) pWarnings += sprintf(pWarnings, ",");
                    pWarnings += sprintf(pWarnings, "{\"code\":0,\"message\":\"Bad layer parameters\"}");
                }
                continue;
            }

            char* params = strtok(token, "|");
            if (!params) continue;

            snprintf(pPath, BUF_SIZE, "%s%s", ROOT, params);

            int valid = 1;
            for (k = 0; k < MAX_PARAMS; k++) {
                params = strtok(NULL, "|");
                if (!params) {
                    valid = 0;
                    break;
                }
                args[k] = atoi(params);
            }

            if (!valid) {
                if (strlen(warnings_buf) + 60 < STRINGS_BUF) {
                    if (warnings++) pWarnings += sprintf(pWarnings, ",");
                    pWarnings += sprintf(pWarnings, "{\"code\":1,\"message\":\"Too few layer parameters\"}");
                }
                continue;
            }

            try {
                index = image->AddLayer(pPath, args[0], args[1], args[2], args[3], args[4]);
                if (args[5] > 0 && args[5] != 100) {
                    Filters::Resize(image->GetLayer(index), args[5], BILINEAR);
                }
            } catch (Exception& e) {
                size_t elen = strlen(e.getMessage());
                if (strlen(errors_buf) + elen + 30 < STRINGS_BUF) {
                    if (errors++) pErrors += sprintf(pErrors, ",");
                    pErrors += sprintf(pErrors, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
                }
            }

        } else if (strcmp(token, "out") == 0) {
            token = strtok(NULL, "=");
            if (!token || !make_output_path(token, pOut, BUF_SIZE)) {
                if (strlen(warnings_buf) + 60 < STRINGS_BUF) {
                    if (warnings++) pWarnings += sprintf(pWarnings, ",");
                    pWarnings += sprintf(pWarnings, "{\"code\":2,\"message\":\"Invalid output filename\"}");
                }
                continue;
            }
            makedirs(pOut);

        } else if (strcmp(token, "number") == 0) {
            token = strtok(NULL, "=");
            if (!token) { /* error */ continue; }
            snprintf(pCIn, BUF_SIZE, "%s", token);

        } else if (strcmp(token, "source") == 0) {
            token = strtok(NULL, "=");
            if (!token) { /* error */ continue; }
            snprintf(pCOut, BUF_SIZE, "%s%s", ROOT, token);

        } else if (strcmp(token, "type") == 0) {
            token = strtok(NULL, "=");
            if (!token) { /* error */ continue; }
            type = (byte)atoi(token);

        } else if (strcmp(token, "exists") == 0) {
            token = strtok(NULL, "=");
            if (!token || !make_output_path(token, pCIn, BUF_SIZE)) {
                /* error */
                continue;
            }
            if (stat(pCIn, &status) != 0) {
                if (strlen(errors_buf) + 50 < STRINGS_BUF) {
                    if (errors++) pErrors += sprintf(pErrors, ",");
                    pErrors += sprintf(pErrors, "{\"code\":10,\"message\":\"File not found\"}");
                }
            }

        } else if (strcmp(token, "copy") == 0) {
            token = strtok(NULL, "=");
            if (!token) { /* warning */ continue; }
            char* src = strtok(token, "|");
            char* dst = strtok(NULL, "|");
            if (!src || !dst) { /* warning */ continue; }

            char src_path[BUF_SIZE], dst_path[BUF_SIZE];
            if (!make_output_path(src, src_path, BUF_SIZE) || !make_output_path(dst, dst_path, BUF_SIZE)) {
                if (strlen(warnings_buf) + 60 < STRINGS_BUF) {
                    if (warnings++) pWarnings += sprintf(pWarnings, ",");
                    pWarnings += sprintf(pWarnings, "{\"code\":3,\"message\":\"Invalid copy path\"}");
                }
                continue;
            }

            try {
                copy(src_path, dst_path);
            } catch (Exception& e) {
                size_t elen = strlen(e.getMessage());
                if (strlen(errors_buf) + elen + 30 < STRINGS_BUF) {
                    if (errors++) pErrors += sprintf(pErrors, ",");
                    pErrors += sprintf(pErrors, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
                }
            }

        } else if (strcmp(token, "delete") == 0) {
            token = strtok(NULL, "=");
            if (!token) { /* warning */ continue; }
            char del_path[BUF_SIZE];
            if (!make_output_path(token, del_path, BUF_SIZE)) {
                if (strlen(warnings_buf) + 60 < STRINGS_BUF) {
                    if (warnings++) pWarnings += sprintf(pWarnings, ",");
                    pWarnings += sprintf(pWarnings, "{\"code\":5,\"message\":\"Invalid delete path\"}");
                }
                continue;
            }
            try {
                delete_file(del_path);
            } catch (Exception& e) {
                size_t elen = strlen(e.getMessage());
                if (strlen(warnings_buf) + elen + 30 < STRINGS_BUF) {
                    if (warnings++) pWarnings += sprintf(pWarnings, ",");
                    pWarnings += sprintf(pWarnings, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
                }
            }
        }
    }

    // Генерация номера
    if (type < 255) {
        image->Clear();
        image->AddBackground(pCOut, FROM_IMAGE, 0x0);
        Number* number = new Number(type, image);
        number->Generate(pCIn)->Save(pOut);
        image->Clear();
        delete number;
    }

    // Сохранение результата
    if (image && image->IsImageExists()) {
        image->Flatten();
        try {
            image->Save(pOut);
        } catch (Exception& e) {
            size_t elen = strlen(e.getMessage());
            if (strlen(errors_buf) + elen + 30 < STRINGS_BUF) {
                if (errors++) pErrors += sprintf(pErrors, ",");
                pErrors += sprintf(pErrors, "{\"code\":%d,\"message\":\"%s\"}", e.getCode(), e.getMessage());
            }
        }
    }

    delete image;

    // Вывод результата
    if (errors > 0) {
        printf("{\"status\":\"ERROR\",\"errors\":[%s]", errors_buf);
    } else {
        printf("{\"status\":\"OK\"");
    }

    if (warnings > 0) {
        printf(",\"warnings\":[%s]}", warnings_buf);
    } else {
        printf("}");
    }

    return 0;
}