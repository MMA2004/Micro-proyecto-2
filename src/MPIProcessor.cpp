#include "MPIProcessor.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include "Timer.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <algorithm>

Image* MPIProcessor::process_filter(const Image* src, const Filter* filter, int rank, int num_procs) {
    if (rank == 0) {
        return master_distribute_and_gather(src, filter, num_procs);
    } else {
        worker_compute(rank, filter);
        return nullptr;
    }
}

Image* MPIProcessor::master_distribute_and_gather(const Image* src, const Filter* filter, int num_procs) {
    if (!src || !filter) return nullptr;

    int width = src->get_width();
    int height = src->get_height();
    int channels = src->get_channels();
    int max_val = src->get_max_val();

    // 1. Difusión de metadatos globales de imagen a todos los nodos
    ImageMetadataMsg meta;
    meta.width = width;
    meta.height = height;
    meta.channels = channels;
    meta.max_val = max_val;
    meta.is_pgm = (channels == 1) ? 1 : 0;

    MPI_Bcast(&meta, sizeof(ImageMetadataMsg), MPI_BYTE, 0, MPI_COMM_WORLD);

    Image* dst = src->clone();

    // Arreglo plano de métricas por nodo (sin std::vector)
    NodeTimingMsg* timings = new NodeTimingMsg[num_procs];

    // Partición de filas por nodo
    int base_rows = height / num_procs;
    int remainder = height % num_procs;

    // Arreglos de límites calculados para cada rango
    int* start_y_arr = new int[num_procs];
    int* num_rows_arr = new int[num_procs];

    for (int k = 0; k < num_procs; ++k) {
        start_y_arr[k] = k * base_rows + std::min(k, remainder);
        num_rows_arr[k] = base_rows + (k < remainder ? 1 : 0);
    }

    // 2. Enviar franjas con halos a cada nodo trabajador (rank > 0)
    for (int k = 1; k < num_procs; ++k) {
        int s_y = start_y_arr[k];
        int n_rows = num_rows_arr[k];
        int e_y = s_y + n_rows;

        bool has_top_halo = (s_y > 0);
        bool has_bottom_halo = (e_y < height);

        int halo_start_y = has_top_halo ? (s_y - 1) : s_y;
        int halo_end_y = has_bottom_halo ? (e_y + 1) : e_y;
        int total_received = halo_end_y - halo_start_y;
        int offset_y = has_top_halo ? 1 : 0;

        ChunkHeaderMsg chunk_header;
        chunk_header.start_y = s_y;
        chunk_header.num_valid_rows = n_rows;
        chunk_header.total_received_rows = total_received;
        chunk_header.offset_y = offset_y;

        // Enviar encabezado de franja
        MPI_Send(&chunk_header, sizeof(ChunkHeaderMsg), MPI_BYTE, k, MPITags::TAG_CHUNK_HEADER, MPI_COMM_WORLD);

        // Enviar bloque de píxeles correspondiente (incluyendo halos)
        size_t bytes_to_send = static_cast<size_t>(total_received) * width * channels;
        const unsigned char* src_ptr = src->get_raw_data() + (static_cast<size_t>(halo_start_y) * width * channels);
        MPI_Send(src_ptr, bytes_to_send, MPI_UNSIGNED_CHAR, k, MPITags::TAG_CHUNK_DATA, MPI_COMM_WORLD);
    }

    // 3. El nodo maestro (Rank 0) procesa su propia franja
    Timer master_timer;
    master_timer.start();

    int master_start_y = start_y_arr[0];
    int master_num_rows = num_rows_arr[0];
    int master_end_y = master_start_y + master_num_rows;

    filter->apply_region(src, dst, 0, width, master_start_y, master_end_y);

    master_timer.stop();

    timings[0].rank = 0;
    timings[0].cpu_time_ms = master_timer.get_cpu_time_ms();
    timings[0].wall_time_ms = master_timer.get_wall_time_ms();

    // 4. Recibir resultados y métricas de cada trabajador
    for (int k = 1; k < num_procs; ++k) {
        int s_y = start_y_arr[k];
        int n_rows = num_rows_arr[k];
        size_t expected_bytes = static_cast<size_t>(n_rows) * width * channels;

        unsigned char* dest_ptr = dst->get_raw_data() + (static_cast<size_t>(s_y) * width * channels);
        MPI_Recv(dest_ptr, expected_bytes, MPI_UNSIGNED_CHAR, k, MPITags::TAG_RESULT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(&timings[k], sizeof(NodeTimingMsg), MPI_BYTE, k, MPITags::TAG_TIMINGS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // 5. Imprimir reporte consolidado de tiempos por nodo
    std::cerr << "============================================================\n"
              << " Reporte de Tiempos MPI por Nodo (Memoria Distribuida)\n"
              << " Filtro: " << filter->get_name() << "\n"
              << "============================================================\n";

    double max_wall_time = 0.0;
    for (int i = 0; i < num_procs; ++i) {
        std::cerr << "  - Nodo " << timings[i].rank << (timings[i].rank == 0 ? " [Maestro]: " : " [Worker]:  ")
                  << "CPU = " << std::fixed << std::setprecision(4) << timings[i].cpu_time_ms << " ms | "
                  << "Wall = " << timings[i].wall_time_ms << " ms\n";
        if (timings[i].wall_time_ms > max_wall_time) {
            max_wall_time = timings[i].wall_time_ms;
        }
    }
    std::cerr << "------------------------------------------------------------\n"
              << " Tiempo de calculo paralelo efectivo (Max Wall): " << max_wall_time << " ms\n"
              << "============================================================\n";

    delete[] timings;
    delete[] start_y_arr;
    delete[] num_rows_arr;

    return dst;
}

void MPIProcessor::worker_compute(int rank, const Filter* filter) {
    if (!filter) return;

    // 1. Recibir metadatos globales
    ImageMetadataMsg meta;
    MPI_Bcast(&meta, sizeof(ImageMetadataMsg), MPI_BYTE, 0, MPI_COMM_WORLD);

    // 2. Recibir encabezado de la franja
    ChunkHeaderMsg chunk_header;
    MPI_Recv(&chunk_header, sizeof(ChunkHeaderMsg), MPI_BYTE, 0, MPITags::TAG_CHUNK_HEADER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // 3. Recibir el buffer de píxeles (con halos)
    size_t total_bytes = static_cast<size_t>(chunk_header.total_received_rows) * meta.width * meta.channels;
    unsigned char* received_buffer = new unsigned char[total_bytes];
    MPI_Recv(received_buffer, total_bytes, MPI_UNSIGNED_CHAR, 0, MPITags::TAG_CHUNK_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Crear sub-imagen local
    Image* sub_src = nullptr;
    if (meta.is_pgm) {
        sub_src = new PGMImage(meta.width, chunk_header.total_received_rows, meta.max_val);
    } else {
        sub_src = new PPMImage(meta.width, chunk_header.total_received_rows, meta.max_val);
    }

    std::memcpy(sub_src->get_raw_data(), received_buffer, total_bytes);
    delete[] received_buffer;

    Image* sub_dst = sub_src->clone();

    // 4. Medir tiempo local de cómputo y aplicar el filtro
    Timer local_timer;
    local_timer.start();

    int start_y = chunk_header.offset_y;
    int end_y = chunk_header.offset_y + chunk_header.num_valid_rows;

    filter->apply_region(sub_src, sub_dst, 0, meta.width, start_y, end_y);

    local_timer.stop();

    // 5. Devolver al maestro únicamente las filas válidas procesadas (sin los halos)
    unsigned char* valid_result_ptr = sub_dst->get_raw_data() + (static_cast<size_t>(start_y) * meta.width * meta.channels);
    size_t valid_bytes = static_cast<size_t>(chunk_header.num_valid_rows) * meta.width * meta.channels;

    MPI_Send(valid_result_ptr, valid_bytes, MPI_UNSIGNED_CHAR, 0, MPITags::TAG_RESULT, MPI_COMM_WORLD);

    // 6. Enviar reporte de tiempos a Rank 0
    NodeTimingMsg timing;
    timing.rank = rank;
    timing.cpu_time_ms = local_timer.get_cpu_time_ms();
    timing.wall_time_ms = local_timer.get_wall_time_ms();

    MPI_Send(&timing, sizeof(NodeTimingMsg), MPI_BYTE, 0, MPITags::TAG_TIMINGS, MPI_COMM_WORLD);

    delete sub_src;
    delete sub_dst;
}
