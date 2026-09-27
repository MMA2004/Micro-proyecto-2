#include "PPMImage.h"

PPMImage::PPMImage(int w, int h, int max_v)
    : Image(w, h, max_v, 3) {
}

PPMImage::PPMImage(const PPMImage& other)
    : Image(other) {
}

const char* PPMImage::get_magic_number() const {
    return "P3";
}

Image* PPMImage::clone() const {
    return new PPMImage(*this);
}

Image* PPMImage::extract_region(int start_x, int start_y, int region_w, int region_h) const {
    if (region_w <= 0 || region_h <= 0) return nullptr;

    PPMImage* sub = new PPMImage(region_w, region_h, max_val);
    for (int y = 0; y < region_h; ++y) {
        int src_y = start_y + y;
        for (int x = 0; x < region_w; ++x) {
            int src_x = start_x + x;
            for (int c = 0; c < 3; ++c) {
                int pixel_val = get_pixel(src_x, src_y, c);
                sub->set_pixel(x, y, pixel_val, c);
            }
        }
    }
    return sub;
}
