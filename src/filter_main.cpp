#include "ImageIO.h"
#include "CharUtils.h"
#include "Timer.h"
#include "BlurFilter.h"
#include "LaplaceFilter.h"
#include "SharpenFilter.h"
#include "SobelFilter.h"
#include <iostream>

namespace {
    const int MAX_FILTERS = 32;

    void print_usage(const char* prog_name) {
        std::cerr << "Uso de " << (prog_name ? prog_name : "filterer") << ":\n"
                  << "  " << (prog_name ? prog_name : "filterer") << " <input.ppm/pgm> <output.ppm/pgm> --f <filtro1> [--f <filtro2> ...]\n\n"
                  << "Filtros disponibles:\n"
                  << "  blur       : Suavizado promedio 3x3\n"
                  << "  gaussian   : Desenfoque gaussiano 3x3\n"
                  << "  laplace    : Deteccion de bordes laplaciano 3x3\n"
                  << "  sharpen    : Realce (sharpening) 3x3\n"
                  << "  sobel      : Magnitud de gradiente Sobel 3x3\n\n"
                  << "Ejemplo:\n"
                  << "  ./filterer fruit.ppm fruit_blur.ppm --f blur\n"
                  << "  ./filterer fruit.ppm fruit_multi.ppm --f blur --f sharpen\n";
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
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    // Comprobar flag de ayuda
    if (CharUtils::equals(argv[1], "--help") || CharUtils::equals(argv[1], "-h")) {
        print_usage(argv[0]);
        return 0;
    }

    const char* input_filepath = argv[1];
    const char* output_filepath = argv[2];

    Filter* filters[MAX_FILTERS];
    int filter_count = 0;

    // Procesar flags de filtros (--f <nombre>) sin usar std::vector ni std::string
    for (int i = 3; i < argc; ++i) {
        if (CharUtils::equals(argv[i], "--f") || CharUtils::equals(argv[i], "-f")) {
            if (i + 1 < argc) {
                const char* filter_name = argv[i + 1];
                Filter* f = create_filter_by_name(filter_name);
                if (f) {
                    if (filter_count < MAX_FILTERS) {
                        filters[filter_count++] = f;
                    } else {
                        std::cerr << "[Advertencia] Limite maximo de filtros alcanzado (" << MAX_FILTERS << ").\n";
                    }
                } else {
                    std::cerr << "[Error] Filtro desconocido: '" << filter_name << "'.\n";
                    print_usage(argv[0]);
                    // Liberar los ya creados
                    for (int k = 0; k < filter_count; ++k) delete filters[k];
                    return 1;
                }
                ++i; // Saltar el argumento del nombre del filtro
            } else {
                std::cerr << "[Error] Se esperaba el nombre del filtro tras la opcion '--f'.\n";
                for (int k = 0; k < filter_count; ++k) delete filters[k];
                return 1;
            }
        }
    }

    if (filter_count == 0) {
        std::cerr << "[Error] Debe especificar al menos un filtro con '--f <nombre>'.\n";
        print_usage(argv[0]);
        return 1;
    }

    // 1. Cargar imagen de entrada
    std::cerr << "[Filterer] Leyendo archivo de entrada: " << input_filepath << "...\n";
    Image* original_img = ImageIO::read_from_file(input_filepath);
    if (!original_img) {
        std::cerr << "[Error] No se pudo cargar la imagen desde: " << input_filepath << "\n";
        for (int k = 0; k < filter_count; ++k) delete filters[k];
        return 1;
    }

    std::cerr << "[Filterer] Imagen cargada:\n"
              << "  - Tipo:        " << original_img->get_magic_number() << "\n"
              << "  - Resolucion:  " << original_img->get_width() << " x " << original_img->get_height() << "\n"
              << "  - Canales:     " << original_img->get_channels() << "\n";

    // 2. Medir tiempo de procesamiento (excluyendo E/S de disco para medir el algoritmo de filtrado puro)
    Timer timer;
    std::cerr << "[Filterer] Aplicando secuencia de " << filter_count << " filtro(s)...\n";
    timer.start();

    Image* current_img = original_img;
    for (int k = 0; k < filter_count; ++k) {
        std::cerr << "  -> Paso " << (k + 1) << "/" << filter_count << ": " << filters[k]->get_name() << "\n";
        Image* filtered_img = filters[k]->apply(current_img);

        if (current_img != original_img) {
            delete current_img; // Liberar imagen intermedia previa
        }
        current_img = filtered_img;
    }

    timer.stop();

    // 3. Imprimir reporte de tiempos de CPU y Wall-clock
    timer.print_report("Filtrado Secuencial (Diseno 2)");

    // 4. Guardar imagen resultante en disco
    std::cerr << "[Filterer] Guardando imagen resultante en: " << output_filepath << "...\n";
    bool success = ImageIO::write_to_file(current_img, output_filepath);

    if (!success) {
        std::cerr << "[Error] Fallo al guardar la imagen en: " << output_filepath << "\n";
    } else {
        std::cerr << "[Filterer] Procesamiento completado exitosamente.\n";
    }

    // 5. Liberar recursos
    if (current_img != original_img) {
        delete current_img;
    }
    delete original_img;

    for (int k = 0; k < filter_count; ++k) {
        delete filters[k];
    }

    return success ? 0 : 1;
}
