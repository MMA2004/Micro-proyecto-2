#ifndef MPI_TYPES_H
#define MPI_TYPES_H

/**
 * @brief Tags de comunicación para MPI (sin std::vector ni std::string).
 */
namespace MPITags {
    const int TAG_META         = 100;
    const int TAG_CHUNK_HEADER = 101;
    const int TAG_CHUNK_DATA   = 102;
    const int TAG_RESULT       = 103;
    const int TAG_TIMINGS      = 104;
}

/**
 * @brief Identificadores de filtros compatibles con MPI.
 */
enum MPIFilterType {
    MPI_FILTER_NONE     = 0,
    MPI_FILTER_BLUR     = 1,
    MPI_FILTER_GAUSSIAN = 2,
    MPI_FILTER_LAPLACE  = 3,
    MPI_FILTER_SHARPEN  = 4,
    MPI_FILTER_SOBEL    = 5
};

/**
 * @brief Metadatos globales de la imagen transmitidos por el Maestro a todos los nodos.
 * Estructura POD (Plain Old Data) de tamaño fijo para MPI_Bcast.
 */
struct ImageMetadataMsg {
    int width;
    int height;
    int channels;
    int max_val;
    int is_pgm; // 1 si es P2 (PGM), 0 si es P3 (PPM)
};

/**
 * @brief Metadatos de la franja enviada a un nodo específico.
 */
struct ChunkHeaderMsg {
    int start_y;             // Fila inicial real en la imagen global
    int num_valid_rows;      // Cantidad de filas calculadas válidas
    int total_received_rows; // Filas totales incluyendo los halos de borde
    int offset_y;            // Desplazamiento local de la primera fila válida (0 o 1)
};

/**
 * @brief Tiempos de ejecución individuales registrados por cada nodo.
 */
struct NodeTimingMsg {
    int rank;
    double cpu_time_ms;
    double wall_time_ms;
};

#endif // MPI_TYPES_H
