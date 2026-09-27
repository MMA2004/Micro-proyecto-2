#ifndef LAPLACE_FILTER_H
#define LAPLACE_FILTER_H

#include "ConvolutionFilter.h"

/**
 * @brief Filtro de detección de bordes Laplaciano (Laplace).
 * Basado en la máscara de detección de bordes vista en clase.
 */
class LaplaceFilter : public ConvolutionFilter {
public:
    LaplaceFilter();

    const char* get_name() const override {
        return "Laplace (Deteccion de bordes)";
    }
};

#endif // LAPLACE_FILTER_H
