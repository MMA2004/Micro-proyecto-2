#include "ImageIO.h"
#include "CharUtils.h"
#include "Timer.h"
#include "OpenMPProcessor.h"
#include "BlurFilter.h"
#include "LaplaceFilter.h"
#include "SharpenFilter.h"
#include "SobelFilter.h"
#include <iostream>
#include <cstdlib>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace {
    const int MAX_FILTERS = 32;

    void print_usage(const char* prog_name) {
        std::cerr << "Uso de " << (prog_name ? prog_name : "omp_filterer") << ":\n"
                  << "  " << (prog_name ? prog_name : "omp_filterer") << " <input.ppm/pgm> <output.ppm/pgm> [--f <filtro1> ...] [--t <hilos>]\n\n"
                  << "Filtros disponibles:\n"
                  << "  blur       : Suavizado promedio 3x3\n"
                  << "  gaussian   : Desenfoque gaussiano 3x3\n"
                  << "  laplace    : Deteccion de bordes laplaciano 3x3\n"
                  << "  sharpen    : Realce (sharpening) 3x3\n"
                  << "  sobel      : Magnitud de gradiente Sobel 3x3\n\n"
                  << "Nota: Si no se especifica '--f', se aplican los tres filtros principales (blur, laplace, sharpen).\n"
                  << "Ejemplo:\n"
                  << "  ./omp_filterer sulfur.pgm sulfur_N.pgm\n"
                  << "  ./omp_filterer sulfur.pgm sulfur_blur.pgm --f blur --t 4\n";
    }

    Filter* create_filter_by_name(const char* name) {
        if (!name) return nullptr;
        if (CharUtils::equals(name, "blur") || CharUtils::equals(name, "suavizado")) {
            return new BlurFilter(false);
        }
        if (CharUtils::equals(name, "gaussian") || CharUtils::equals(name, "desenfoque")) {
            return new BlurFilter(true);
        }
        if (CharUtils::equals(name, "laplace") || CharUtils::equals(name, "bordes")) {
            return new LaplaceFilter();
        }
        if (CharUtils::equals(name, "sharpen") || CharUtils::equals(name, "realce")) {
            return new SharpenFilter();
        }
        if (CharUtils::equals(name, "sobel")) {
            return new SobelFilter();
        }
        return nullptr;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    if (CharUtils::equals(argv[1], "--help") || CharUtils::equals(argv[1], "-h")) {
        print_usage(argv[0]);
        return 0;
    }

    const char* input_filepath = argv[1];
    const char* output_filepath = argv[2];

    Filter* filters[MAX_FILTERS];
    int filter_count = 0;
    int num_threads = 0;

    for (int i = 3; i < argc; ++i) {
        if (CharUtils::equals(argv[i], "--f") || CharUtils::equals(argv[i], "-f")) {
            if (i + 1 < argc) {
                const char* filter_name = argv[i + 1];
                Filter* f = create_filter_by_name(filter_name);
                if (f) {
                    if (filter_count < MAX_FILTERS) {
                        filters[filter_count++] = f;
                    }
                } else {
                    std::cerr << "[Error] Filtro desconocido: '" << filter_name << "'.\n";
                    for (int k = 0; k < filter_count; ++k) delete filters[k];
                    return 1;
                }
                ++i;
            }
        } else if (CharUtils::equals(argv[i], "--t") || CharUtils::equals(argv[i], "-t")) {
            if (i + 1 < argc) {
                num_threads = std::atoi(argv[i + 1]);
                ++i;
            }
        }
    }

    // Si no se proporcionaron filtros explícitamente, aplicar los 3 estándar de la guía
    if (filter_count == 0) {
        std::cerr << "[omp_filterer] Sin filtro especificado. Aplicando filtros estándar (blur, laplace, sharpen)...\n";
        filters[filter_count++] = new BlurFilter();
        filters[filter_count++] = new LaplaceFilter();
        filters[filter_count++] = new SharpenFilter();
    }

    std::cerr << "[omp_filterer] Leyendo archivo de entrada: " << input_filepath << "...\n";
    Image* original_img = ImageIO::read_from_file(input_filepath);
    if (!original_img) {
        std::cerr << "[Error] No se pudo cargar la imagen desde: " << input_filepath << "\n";
        for (int k = 0; k < filter_count; ++k) delete filters[k];
        return 1;
    }

    std::cerr << "[omp_filterer] Imagen cargada:\n"
              << "  - Tipo:        " << original_img->get_magic_number() << "\n"
              << "  - Resolucion:  " << original_img->get_width() << " x " << original_img->get_height() << "\n"
              << "  - Canales:     " << original_img->get_channels() << "\n";

    Timer timer;
    std::cerr << "[omp_filterer] Iniciando procesamiento con OpenMP...\n";
    timer.start();

    Image* current_img = original_img;
    for (int k = 0; k < filter_count; ++k) {
        std::cerr << "  -> Filtro " << (k + 1) << "/" << filter_count << ": " << filters[k]->get_name() << "\n";
        Image* filtered_img = OpenMPProcessor::process_openmp(current_img, filters[k], num_threads, true);

        if (current_img != original_img) {
            delete current_img;
        }
        current_img = filtered_img;
    }

    timer.stop();
    timer.print_report("Paralelo OpenMP");

    std::cerr << "[omp_filterer] Guardando imagen resultante en: " << output_filepath << "...\n";
    bool success = ImageIO::write_to_file(current_img, output_filepath);

    if (current_img != original_img) {
        delete current_img;
    }
    delete original_img;

    for (int k = 0; k < filter_count; ++k) {
        delete filters[k];
    }

    return success ? 0 : 1;
}
