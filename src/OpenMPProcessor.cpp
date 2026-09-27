#include "OpenMPProcessor.h"
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

Image* OpenMPProcessor::process_openmp(const Image* src, const Filter* filter, int num_threads, bool verbose) {
    if (!src || !filter) return nullptr;

    Image* dst = src->clone();
    int width = src->get_width();
    int height = src->get_height();

#ifdef _OPENMP
    if (num_threads > 0) {
        omp_set_num_threads(num_threads);
    }

    int threads_used = (num_threads > 0) ? num_threads : omp_get_max_threads();
    if (verbose) {
        std::cerr << "[OpenMP] Ejecutando bucle paralelo con " << threads_used << " hilos...\n";
    }

    #pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        filter->apply_region(src, dst, 0, width, y, y + 1);
    }
#else
    if (verbose) {
        std::cerr << "[OpenMP] Aviso: Compilado sin soporte OpenMP (-fopenmp). Ejecutando fallback.\n";
    }
    filter->apply_region(src, dst, 0, width, 0, height);
#endif

    return dst;
}
