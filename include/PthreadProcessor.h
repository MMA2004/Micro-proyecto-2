#ifndef PTHREAD_PROCESSOR_H
#define PTHREAD_PROCESSOR_H

#include "Image.h"
#include "Filter.h"
#include <pthread.h>

/**
 * @brief Información asignada a cada uno de los 4 hilos para procesar su cuadrante.
 * Sin uso de std::vector ni std::string.
 */
struct ThreadRegionData {
    const Filter* filter;
    const Image* src;
    Image* dst;
    int start_x;
    int end_x;
    int start_y;
    int end_y;
    int quadrant_id;
    const char* quadrant_name;
};

/**
 * @brief Orquestador de paralelización por 4 cuadrantes usando POSIX Threads (Pthreads).
 */
class PthreadProcessor {
public:
    /**
     * @brief Procesa la imagen dividiéndola en 4 cuadrantes concurrentes con 4 hilos.
     * @param src Imagen original inmutable.
     * @param filter Filtro a aplicar en cada cuadrante.
     * @param verbose Si es true, imprime las coordenadas y detalles de cada hilo/cuadrante.
     * @return Nueva imagen procesada.
     */
    static Image* process_quadrants(const Image* src, const Filter* filter, bool verbose = true);

private:
    static void* worker_routine(void* arg);
};

#endif // PTHREAD_PROCESSOR_H
