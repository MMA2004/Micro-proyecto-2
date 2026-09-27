#include "SharpenFilter.h"

namespace {
    const double SHARPEN_KERNEL[3][3] = {
        { 0.0, -1.0,  0.0},
        {-1.0,  5.0, -1.0},
        { 0.0, -1.0,  0.0}
    };
}

SharpenFilter::SharpenFilter()
    : ConvolutionFilter(SHARPEN_KERNEL, 1.0, 0.0) {
}
