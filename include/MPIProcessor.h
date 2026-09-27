#ifndef MPI_PROCESSOR_H
#define MPI_PROCESSOR_H

#include "Image.h"
#include "Filter.h"
#include "MPITypes.h"
#include <mpi.h>

/**
 * @brief Orquestador de memoria distribuida con paso de mensajes MPI.
 * Divide la imagen en franjas horizontales con halos de 1 fila de frontera.
 * Sin uso de std::vector ni std::string.
 */
class MPIProcessor {
public:
    /**
     * @brief Ejecuta el filtrado distribuido.
     * En Rank 0: Distribuye las franjas, procesa su cuota, recibe resultados y ensambla la imagen.
     * En Rank > 0: Recibe su franja con halos, procesa localmente y devuelve el resultado a Rank 0.
     * @param src Puntero a la imagen completa (solo válido en Rank 0, puede ser nullptr en workers).
     * @param filter Filtro a aplicar.
     * @param rank Rango del proceso actual en MPI_COMM_WORLD.
     * @param num_procs Número total de procesos en MPI_COMM_WORLD.
     * @return Puntero a la imagen resultante (en Rank 0) o nullptr (en Workers).
     */
    static Image* process_filter(const Image* src, const Filter* filter, int rank, int num_procs);

private:
    static Image* master_distribute_and_gather(const Image* src, const Filter* filter, int num_procs);
    static void worker_compute(int rank, const Filter* filter);
};

#endif // MPI_PROCESSOR_H
