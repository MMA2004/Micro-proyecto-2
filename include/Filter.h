#ifndef FILTER_H
#define FILTER_H

#include "Image.h"

/**
 * @brief Clase base abstracta para cualquier filtro de procesamiento de imágenes.
 * Sigue el principio abierto/cerrado (Open/Closed Principle) para facilitar agregar nuevos filtros.
 */
class Filter {
public:
    virtual ~Filter() = default;

    /**
     * @brief Aplica el filtro sobre una imagen fuente y genera una nueva imagen con el resultado.
     * @param src Puntero a la imagen de entrada (de solo lectura).
     * @return Puntero a una nueva imagen procesada (responsabilidad del llamador liberar la memoria).
     */
    virtual Image* apply(const Image* src) const = 0;

    /**
     * @brief Aplica el filtro sobre una region rectangular [start_x, end_x) x [start_y, end_y).
     * Esencial para la paralelización por cuadrantes en memoria compartida (Pthreads y OpenMP).
     */
    virtual void apply_region(const Image* src, Image* dst, int start_x, int end_x, int start_y, int end_y) const = 0;

    /**
     * @brief Retorna el nombre del filtro.
     */
    virtual const char* get_name() const = 0;
};

#endif // FILTER_H
