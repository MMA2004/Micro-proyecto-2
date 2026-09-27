#include "PGMImage.h"

PGMImage::PGMImage(int w, int h, int max_v)
    : Image(w, h, max_v, 1) {
}

PGMImage::PGMImage(const PGMImage& other)
    : Image(other) {
}

const char* PGMImage::get_magic_number() const {
    return "P2";
}

Image* PGMImage::clone() const {
    return new PGMImage(*this);
}

Image* PGMImage::extract_region(int start_x, int start_y, int region_w, int region_h) const {
    if (region_w <= 0 || region_h <= 0) return nullptr;

    PGMImage* sub = new PGMImage(region_w, region_h, max_val);
    for (int y = 0; y < region_h; ++y) {
        int src_y = start_y + y;
        for (int x = 0; x < region_w; ++x) {
            int src_x = start_x + x;
            int pixel_val = get_pixel(src_x, src_y, 0);
            sub->set_pixel(x, y, pixel_val, 0);
        }
    }
    return sub;
}
