#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <ctime>

/**
 * @brief Clase para medición precisa de tiempos de ejecución:
 * - Tiempo de CPU (consumo de ciclos del procesador via std::clock).
 * - Tiempo Total / Wall-clock (tiempo real de reloj via std::chrono).
 */
class Timer {
private:
    std::chrono::high_resolution_clock::time_point wall_start;
    std::chrono::high_resolution_clock::time_point wall_end;
    std::clock_t cpu_start;
    std::clock_t cpu_end;
    bool running;

public:
    Timer();

    /**
     * @brief Inicia o reinicia la medición de tiempo.
     */
    void start();

    /**
     * @brief Detiene la medición de tiempo.
     */
    void stop();

    /**
     * @brief Retorna el tiempo transcurrido de reloj de pared (wall-clock) en milisegundos.
     */
    double get_wall_time_ms() const;

    /**
     * @brief Retorna el tiempo transcurrido de CPU en milisegundos.
     */
    double get_cpu_time_ms() const;

    /**
     * @brief Retorna el tiempo transcurrido de reloj de pared en segundos.
     */
    double get_wall_time_s() const;

    /**
     * @brief Retorna el tiempo transcurrido de CPU en segundos.
     */
    double get_cpu_time_s() const;

    /**
     * @brief Imprime un reporte formateado de las métricas en stderr.
     * @param label Etiqueta descriptiva (por ejemplo, el nombre del filtro).
     */
    void print_report(const char* label = nullptr) const;
};

#endif // TIMER_H
