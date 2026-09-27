#ifndef OPENMP_PROCESSOR_H
#define OPENMP_PROCESSOR_H

#include "Image.h"
#include "Filter.h"

/**
 * @brief Orquestador de paralelización con directivas OpenMP.
 */
class OpenMPProcessor {
public:
    /**
     * @brief Aplica un filtro sobre la imagen utilizando directivas OpenMP en paralelo a nivel de filas.
     * @param src Imagen original de solo lectura.
     * @param filter Filtro a aplicar.
     * @param num_threads Número de hilos a usar (0 = usar el máximo disponible por el entorno).
     * @param verbose Imprime información de los hilos de OpenMP en ejecución.
     * @return Nueva imagen procesada.
     */
    static Image* process_openmp(const Image* src, const Filter* filter, int num_threads = 0, bool verbose = true);
};

#endif // OPENMP_PROCESSOR_H
