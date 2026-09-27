#include "SobelFilter.h"
#include <cmath>
#include <algorithm>

namespace {
    const double SOBEL_X[3][3] = {
        {-1.0, 0.0, 1.0},
        {-2.0, 0.0, 2.0},
        {-1.0, 0.0, 1.0}
    };

    const double SOBEL_Y[3][3] = {
        {-1.0, -2.0, -1.0},
        { 0.0,  0.0,  0.0},
        { 1.0,  2.0,  1.0}
    };
}

Image* SobelFilter::apply(const Image* src) const {
    if (!src) return nullptr;
    Image* dst = src->clone();
    apply_region(src, dst, 0, src->get_width(), 0, src->get_height());
    return dst;
}

void SobelFilter::apply_region(const Image* src, Image* dst,
                               int start_x, int end_x,
                               int start_y, int end_y) const {
    if (!src || !dst) return;

    int width = src->get_width();
    int height = src->get_height();
    int channels = src->get_channels();
    int max_val = src->get_max_val();

    for (int y = start_y; y < end_y; ++y) {
        if (y < 0 || y >= height) continue;
        for (int x = start_x; x < end_x; ++x) {
            if (x < 0 || x >= width) continue;

            for (int c = 0; c < channels; ++c) {
                double gx = 0.0;
                double gy = 0.0;

                for (int ky = 0; ky < 3; ++ky) {
                    int dy = ky - 1;
                    int sample_y = std::max(0, std::min(y + dy, height - 1));

                    for (int kx = 0; kx < 3; ++kx) {
                        int dx = kx - 1;
                        int sample_x = std::max(0, std::min(x + dx, width - 1));

                        int pixel_val = src->get_pixel(sample_x, sample_y, c);
                        gx += SOBEL_X[ky][kx] * pixel_val;
                        gy += SOBEL_Y[ky][kx] * pixel_val;
                    }
                }

                double magnitude = std::sqrt(gx * gx + gy * gy);
                int int_val = static_cast<int>(std::round(magnitude));
                int clamped = std::max(0, std::min(int_val, max_val));

                dst->set_pixel(x, y, clamped, c);
            }
        }
    }
}
