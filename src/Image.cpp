#include "Image.h"
#include <cstring>
#include <algorithm>
#include <stdexcept>

Image::Image(int w, int h, int max_v, int ch)
    : width(w), height(h), max_val(max_v), channels(ch), data(nullptr) {
    if (width > 0 && height > 0 && channels > 0) {
        size_t total_bytes = static_cast<size_t>(width) * height * channels;
        data = new unsigned char[total_bytes](); // Inicializado a ceros
    }
}

Image::~Image() {
    delete[] data;
    data = nullptr;
}

Image::Image(const Image& other)
    : width(other.width), height(other.height), max_val(other.max_val), channels(other.channels), data(nullptr) {
    if (other.data && width > 0 && height > 0 && channels > 0) {
        size_t total_bytes = static_cast<size_t>(width) * height * channels;
        data = new unsigned char[total_bytes];
        std::memcpy(data, other.data, total_bytes);
    }
}

Image& Image::operator=(const Image& other) {
    if (this != &other) {
        delete[] data;
        data = nullptr;

        width = other.width;
        height = other.height;
        max_val = other.max_val;
        channels = other.channels;

        if (other.data && width > 0 && height > 0 && channels > 0) {
            size_t total_bytes = static_cast<size_t>(width) * height * channels;
            data = new unsigned char[total_bytes];
            std::memcpy(data, other.data, total_bytes);
        }
    }
    return *this;
}

Image::Image(Image&& other) noexcept
    : width(other.width), height(other.height), max_val(other.max_val),
      channels(other.channels), data(other.data) {
    other.data = nullptr;
    other.width = 0;
    other.height = 0;
    other.channels = 0;
    other.max_val = 255;
}

Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) {
        delete[] data;

        width = other.width;
        height = other.height;
        max_val = other.max_val;
        channels = other.channels;
        data = other.data;

        other.data = nullptr;
        other.width = 0;
        other.height = 0;
        other.channels = 0;
        other.max_val = 255;
    }
    return *this;
}

int Image::get_pixel(int x, int y, int c) const {
    if (!data || x < 0 || x >= width || y < 0 || y >= height || c < 0 || c >= channels) {
        return 0;
    }
    size_t index = (static_cast<size_t>(y) * width + x) * channels + c;
    return static_cast<int>(data[index]);
}

void Image::set_pixel(int x, int y, int value, int c) {
    if (!data || x < 0 || x >= width || y < 0 || y >= height || c < 0 || c >= channels) {
        return;
    }
    // Clamping entre 0 y max_val
    int clamped = std::max(0, std::min(value, max_val));
    size_t index = (static_cast<size_t>(y) * width + x) * channels + c;
    data[index] = static_cast<unsigned char>(clamped);
}

void Image::paste_region(int start_x, int start_y, const Image* region) {
    if (!region || !data || !region->get_raw_data()) return;
    if (channels != region->get_channels()) return;

    int reg_w = region->get_width();
    int reg_h = region->get_height();

    for (int ry = 0; ry < reg_h; ++ry) {
        int dest_y = start_y + ry;
        if (dest_y < 0 || dest_y >= height) continue;

        for (int rx = 0; rx < reg_w; ++rx) {
            int dest_x = start_x + rx;
            if (dest_x < 0 || dest_x >= width) continue;

            for (int c = 0; c < channels; ++c) {
                set_pixel(dest_x, dest_y, region->get_pixel(rx, ry, c), c);
            }
        }
    }
}
