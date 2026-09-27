#ifndef SOBEL_FILTER_H
#define SOBEL_FILTER_H

#include "Filter.h"

/**
 * @brief Filtro de detección de bordes Sobel.
 * Combina las derivadas direccionales Gx y Gy: G = sqrt(Gx^2 + Gy^2).
 * Recomendado en las notas de la guía del proyecto.
 */
class SobelFilter : public Filter {
public:
    SobelFilter() = default;
    ~SobelFilter() override = default;

    Image* apply(const Image* src) const override;

    const char* get_name() const override {
        return "Sobel (Magnitud de Gradiente)";
    }
};

#endif // SOBEL_FILTER_H
