#include "BlurFilter.h"

namespace {
    const double BOX_KERNEL[3][3] = {
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}
    };

    const double GAUSSIAN_KERNEL[3][3] = {
        {1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0},
        {2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0},
        {1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}
    };
}

BlurFilter::BlurFilter(bool gaussian)
    : ConvolutionFilter(gaussian ? GAUSSIAN_KERNEL : BOX_KERNEL, 1.0, 0.0),
      is_gaussian(gaussian) {
}
