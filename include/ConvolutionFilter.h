#ifndef CONVOLUTION_FILTER_H
#define CONVOLUTION_FILTER_H

#include "Filter.h"

/**
 * @brief Filtro de convolución 2D con kernel 3x3 usando arreglos continuos nativos.
 * Sin uso de std::vector. Compatible con imágenes de 1 canal (PGM) y 3 canales (PPM).
 */
class ConvolutionFilter : public Filter {
protected:
    double kernel[3][3];
    double factor;
    double bias;

public:
    /**
     * @brief Constructor con matriz 3x3, factor multiplicador y sesgo (bias).
     */
    ConvolutionFilter(const double k[3][3], double f = 1.0, double b = 0.0);

    ~ConvolutionFilter() override = default;

    /**
     * @brief Aplica la convolución 3x3 sobre la imagen.
     */
    Image* apply(const Image* src) const override;

    /**
     * @brief Aplica la convolución 3x3 sobre una región rectangular específica.
     */
    void apply_region(const Image* src, Image* dst, int start_x, int end_x, int start_y, int end_y) const override;

    const char* get_name() const override { return "Convolucion 3x3"; }
};

#endif // CONVOLUTION_FILTER_H
