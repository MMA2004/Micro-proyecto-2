#include "PthreadProcessor.h"
#include <iostream>

void* PthreadProcessor::worker_routine(void* arg) {
    ThreadRegionData* data = static_cast<ThreadRegionData*>(arg);
    if (!data || !data->filter || !data->src || !data->dst) {
        return nullptr;
    }

    data->filter->apply_region(data->src, data->dst,
                               data->start_x, data->end_x,
                               data->start_y, data->end_y);

    return nullptr;
}

Image* PthreadProcessor::process_quadrants(const Image* src, const Filter* filter, bool verbose) {
    if (!src || !filter) return nullptr;

    Image* dst = src->clone();
    int width = src->get_width();
    int height = src->get_height();

    int mid_x = width / 2;
    int mid_y = height / 2;

    // Arreglo estático de 4 tareas de cuadrantes (sin std::vector)
    ThreadRegionData tasks[4];
    pthread_t threads[4];

    // Cuadrante 0: Arriba-Izquierda
    tasks[0].filter = filter;
    tasks[0].src = src;
    tasks[0].dst = dst;
    tasks[0].start_x = 0;
    tasks[0].end_x = mid_x;
    tasks[0].start_y = 0;
    tasks[0].end_y = mid_y;
    tasks[0].quadrant_id = 0;
    tasks[0].quadrant_name = "Arriba-Izquierda (Top-Left)";

    // Cuadrante 1: Arriba-Derecha
    tasks[1].filter = filter;
    tasks[1].src = src;
    tasks[1].dst = dst;
    tasks[1].start_x = mid_x;
    tasks[1].end_x = width;
    tasks[1].start_y = 0;
    tasks[1].end_y = mid_y;
    tasks[1].quadrant_id = 1;
    tasks[1].quadrant_name = "Arriba-Derecha (Top-Right)";

    // Cuadrante 2: Abajo-Izquierda
    tasks[2].filter = filter;
    tasks[2].src = src;
    tasks[2].dst = dst;
    tasks[2].start_x = 0;
    tasks[2].end_x = mid_x;
    tasks[2].start_y = mid_y;
    tasks[2].end_y = height;
    tasks[2].quadrant_id = 2;
    tasks[2].quadrant_name = "Abajo-Izquierda (Bottom-Left)";

    // Cuadrante 3: Abajo-Derecha
    tasks[3].filter = filter;
    tasks[3].src = src;
    tasks[3].dst = dst;
    tasks[3].start_x = mid_x;
    tasks[3].end_x = width;
    tasks[3].start_y = mid_y;
    tasks[3].end_y = height;
    tasks[3].quadrant_id = 3;
    tasks[3].quadrant_name = "Abajo-Derecha (Bottom-Right)";

    if (verbose) {
        std::cerr << "[Pthreads] Lanzando 4 hilos por cuadrantes:\n";
        for (int i = 0; i < 4; ++i) {
            std::cerr << "  - Hilo " << i << " [" << tasks[i].quadrant_name << "]: "
                      << "X=[" << tasks[i].start_x << ", " << tasks[i].end_x << "), "
                      << "Y=[" << tasks[i].start_y << ", " << tasks[i].end_y << ")\n";
        }
    }

    // Creación de los 4 hilos POSIX
    for (int i = 0; i < 4; ++i) {
        int rc = pthread_create(&threads[i], nullptr, worker_routine, &tasks[i]);
        if (rc != 0) {
            std::cerr << "[Error] Fallo al crear el hilo " << i << " (error code: " << rc << ")\n";
        }
    }

    // Espera a que todos los hilos concluyan (Join)
    for (int i = 0; i < 4; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return dst;
}
