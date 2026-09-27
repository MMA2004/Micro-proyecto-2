#include "LaplaceFilter.h"

namespace {
    const double LAPLACE_KERNEL[3][3] = {
        {-1.0, -1.0, -1.0},
        {-1.0,  8.0, -1.0},
        {-1.0, -1.0, -1.0}
    };
}

LaplaceFilter::LaplaceFilter()
    : ConvolutionFilter(LAPLACE_KERNEL, 1.0, 0.0) {
}
