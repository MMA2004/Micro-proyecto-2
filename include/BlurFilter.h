#ifndef BLUR_FILTER_H
#define BLUR_FILTER_H

#include "ConvolutionFilter.h"

/**
 * @brief Filtro de suavizado (Blur) 3x3.
 * Basado en los filtros de promedio y gaussiano vistos en clase.
 */
class BlurFilter : public ConvolutionFilter {
private:
    bool is_gaussian;

public:
    /**
     * @brief Constructor del filtro blur.
     * @param gaussian Si es true, usa kernel gaussiano (1/16); si es false, promedio uniforme (1/9).
     */
    BlurFilter(bool gaussian = false);

    const char* get_name() const override {
        return is_gaussian ? "Suavizado Gaussiano (Blur)" : "Suavizado Promedio (Blur)";
    }
};

#endif // BLUR_FILTER_H
