#include "Timer.h"
#include <iostream>
#include <iomanip>

Timer::Timer()
    : wall_start(), wall_end(), cpu_start(0), cpu_end(0), running(false) {
}

void Timer::start() {
    running = true;
    cpu_start = std::clock();
    wall_start = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
    if (running) {
        wall_end = std::chrono::high_resolution_clock::now();
        cpu_end = std::clock();
        running = false;
    }
}

double Timer::get_wall_time_ms() const {
    auto end_time = running ? std::chrono::high_resolution_clock::now() : wall_end;
    std::chrono::duration<double, std::milli> elapsed = end_time - wall_start;
    return elapsed.count();
}

double Timer::get_cpu_time_ms() const {
    std::clock_t current_cpu = running ? std::clock() : cpu_end;
    return (static_cast<double>(current_cpu - cpu_start) / CLOCKS_PER_SEC) * 1000.0;
}

double Timer::get_wall_time_s() const {
    return get_wall_time_ms() / 1000.0;
}

double Timer::get_cpu_time_s() const {
    return get_cpu_time_ms() / 1000.0;
}

void Timer::print_report(const char* label) const {
    std::cerr << "----------------------------------------\n";
    if (label) {
        std::cerr << " Reporte de Tiempo: " << label << "\n";
    } else {
        std::cerr << " Reporte de Tiempo de Ejecucion\n";
    }
    std::cerr << "----------------------------------------\n"
              << std::fixed << std::setprecision(4)
              << " Tiempo de CPU:       " << get_cpu_time_ms() << " ms (" << get_cpu_time_s() << " s)\n"
              << " Tiempo Total (Wall): " << get_wall_time_ms() << " ms (" << get_wall_time_s() << " s)\n"
              << "----------------------------------------\n";
}
