#include "ConvolutionFilter.h"
#include <cmath>
#include <algorithm>

ConvolutionFilter::ConvolutionFilter(const double k[3][3], double f, double b)
    : factor(f), bias(b) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            kernel[i][j] = k[i][j];
        }
    }
}

Image* ConvolutionFilter::apply(const Image* src) const {
    if (!src) return nullptr;
    Image* dst = src->clone();
    apply_region(src, dst, 0, src->get_width(), 0, src->get_height());
    return dst;
}

void ConvolutionFilter::apply_region(const Image* src, Image* dst,
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
                double sum = 0.0;

                for (int ky = 0; ky < 3; ++ky) {
                    int dy = ky - 1;
                    int sample_y = std::max(0, std::min(y + dy, height - 1));

                    for (int kx = 0; kx < 3; ++kx) {
                        int dx = kx - 1;
                        int sample_x = std::max(0, std::min(x + dx, width - 1));

                        int pixel_val = src->get_pixel(sample_x, sample_y, c);
                        sum += kernel[ky][kx] * pixel_val;
                    }
                }

                double final_val = (sum * factor) + bias;
                int int_val = static_cast<int>(std::round(final_val));
                int clamped = std::max(0, std::min(int_val, max_val));

                dst->set_pixel(x, y, clamped, c);
            }
        }
    }
}
