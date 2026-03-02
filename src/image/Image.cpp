#include <merge/Image.h>
#include <merge/FileImage.h>
#include <merge/Filters.h>
#include <merge/utils.h>
#include <cstring>
#include <algorithm>

// Конструктор
Image::Image(void) {
    count = 0;
    avg_grey = AVERAGE_COLOR;
    layers = nullptr;
    bg = nullptr;
}

// Деструктор
Image::~Image(void) {
    Clear();
    delete[] layers;
}

void Image::Clear(void) {
    for (byte i = 0; i < count; ++i) {
        delete layers[i];
    }
    count = 0;
    delete bg;
    bg = nullptr;
}

// AddBackground: из файла
void Image::AddBackground(char* filename, byte flags, size_t color) {
    if (bg != nullptr) {
        delete bg;
    }

    FileImage img;
    Header h = img.Open(filename, flags);
    byte** rows = AllocateMemory<byte>(h.height, h.row_bytes);
    
    byte** src_rows = img.Read();
    for (size_t i = 0; i < h.height; ++i) {
        memcpy(rows[i], src_rows[i], h.row_bytes);
    }

    LayerLoc loc = {0, 0, 0};
    bg = new Layer(h, rows, loc);
    bg->flags = flags;
    bg->bg_color = color;

    img.Close();
}

// AddBackground: из массива строк
void Image::AddBackground(byte** rows, byte flags, size_t color, Header header) {
    if (bg != nullptr) {
        delete bg;
    }
    LayerLoc loc = {0, 0, 0};
    bg = new Layer(header, rows, loc);
    bg->flags = flags;
    bg->bg_color = color;
}

// AddLayer: из файла
byte Image::AddLayer(char* filename, size_t x, size_t y, byte z, byte flags, size_t color) {
    FileImage img;
    Header h = img.Open(filename, flags);
    byte** rows = AllocateMemory<byte>(h.height, h.row_bytes);

    byte** src_rows = img.Read();
    for (size_t i = 0; i < h.height; ++i) {
        memcpy(rows[i], src_rows[i], h.row_bytes);
    }

    LayerLoc loc = {(int)x, (int)y, z};
    Layer* layer = new Layer(h, rows, loc);
    layer->flags = flags;
    layer->bg_color = color;

    Layer** temp = new Layer*[count + 1];
    for (byte i = 0; i < count; ++i) {
        temp[i] = layers[i];
    }
    temp[count] = layer;
    delete[] layers;
    layers = temp;
    return count++;
}

// AddLayer: из данных
byte Image::AddLayer(byte** rows, Header header, size_t x, size_t y, byte z, byte flags, size_t color) {
    LayerLoc loc = {(int)x, (int)y, z};
    Layer* layer = new Layer(header, rows, loc);
    layer->flags = flags;
    layer->bg_color = color;

    Layer** temp = new Layer*[count + 1];
    for (byte i = 0; i < count; ++i) {
        temp[i] = layers[i];
    }
    temp[count] = layer;
    delete[] layers;
    layers = temp;
    return count++;
}

// Flatten: склейка всех слоёв
Image* Image::Flatten(void) {
    if (count == 0 && bg == nullptr) return this;

    size_t max_width = 0, max_height = 0;

    if (bg != nullptr) {
        max_width = std::max(max_width, (size_t)bg->info.width);
        max_height = std::max(max_height, (size_t)bg->info.height);
    }
    for (byte i = 0; i < count; ++i) {
        Header h = layers[i]->info;
        max_width = std::max(max_width, (size_t)(layers[i]->position.x + h.width));
        max_height = std::max(max_height, (size_t)(layers[i]->position.y + h.height));
    }

    if (max_width == 0 || max_height == 0) return this;

    Header out_header;
    out_header.width = max_width;
    out_header.height = max_height;
    out_header.color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    out_header.bit_depth = 8;
    out_header.row_bytes = max_width * 4;

    byte** canvas = AllocateMemory<byte>(max_height, out_header.row_bytes);
    for (size_t y = 0; y < max_height; ++y) {
        memset(canvas[y], 0, out_header.row_bytes);
    }

    // Рисуем фон
    if (bg != nullptr) {
        Header h = bg->info;
        for (size_t y = 0; y < h.height; ++y) {
            memcpy(canvas[y], bg->rows[y], std::min(h.row_bytes, out_header.row_bytes));
        }
    }

    // Рисуем слои
    for (byte i = 0; i < count; ++i) {
        Layer* layer = layers[i];
        Header h = layer->info;
        for (size_t y = 0; y < h.height; ++y) {
            if (layer->position.y + y >= max_height) continue;
            byte* dst = canvas[layer->position.y + y] + layer->position.x * 4;
            byte* src = layer->rows[y];
            for (size_t x = 0; x < h.width; ++x) {
                if (layer->position.x + x >= max_width) continue;
                int alpha = src[x * 4 + 3];
                if (alpha == 255) {
                    memcpy(dst + x * 4, src + x * 4, 4);
                } else if (alpha > 0) {
                    for (int c = 0; c < 3; ++c) {
                        dst[x * 4 + c] = (src[x * 4 + c] * alpha + dst[x * 4 + c] * (255 - alpha)) / 255;
                    }
                }
            }
        }
    }

    Clear();
    delete[] layers;
    layers = nullptr;

    AddBackground(canvas, 0, 0, out_header);
    return this;
}

// Save: сохранение в файл
void Image::Save(char* filename) {
    if (bg == nullptr) return;

    FileImage img;
    if (!img.Create(filename, bg->info)) {
        throw Exception(ERROR_FOPEN, "Cannot create file", filename);
    }
    img.Write(bg->rows);
    img.Close();
}

byte Image::GetLayersCount(void) {
    return count;
}

bool Image::IsImageExists(void) {
    return count > 0 || bg != nullptr;
}

Layer* Image::GetLayer(byte index) {
    if (index >= count) return nullptr;
    return layers[index];
}

Layer* Image::GetBackground(void) {
    return bg;
}

size_t Image::GetGreyscale(size_t r, size_t g, size_t b) {
    return (r * 3 + g * 6 + b) / 10;
}

size_t Image::LineGreyscale(size_t r, size_t g, size_t b) {
    return GetGreyscale(r, g, b);
}

void Image::CreateBackground(void) {
    // заглушка
}

Header Image::GenerateHeader(size_t width, size_t height, short bit_depth) {
    Header header;
    header.width = width;
    header.height = height;
    header.bit_depth = bit_depth;
    header.color_type = PNG_COLOR_TYPE_RGB_ALPHA; // или другой тип, если нужно
    header.row_bytes = width * 4; // для RGBA по 1 байту на канал
    return header;
}

void Image::Create(Header header) {
    this->Clear();
    this->AddBackground(AllocateMemory<byte>(header.height, header.row_bytes), 0, 0, header);
}