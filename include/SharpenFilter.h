#ifndef SHARPEN_FILTER_H
#define SHARPEN_FILTER_H

#include "ConvolutionFilter.h"

/**
 * @brief Filtro de realce (Sharpening / Realce).
 * Basado en el kernel de realce visto en clase.
 */
class SharpenFilter : public ConvolutionFilter {
public:
    SharpenFilter();

    const char* get_name() const override {
        return "Realce (Sharpening)";
    }
};

#endif // SHARPEN_FILTER_H
