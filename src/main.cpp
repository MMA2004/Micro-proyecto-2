#include "ImageIO.h"
#include "CharUtils.h"
#include <iostream>

void print_usage(const char* prog_name) {
    std::cerr << "Uso de " << (prog_name ? prog_name : "processor") << ":\n"
              << "  1. Por argumentos:      ./processor <input.ppm/pgm> <output.ppm/pgm>\n"
              << "  2. Entrada por stdin:   ./processor <output.ppm/pgm> < <input.ppm/pgm>\n"
              << "  3. Tuberia (pipe):      ./processor < <input.ppm/pgm> > <output.ppm/pgm>\n\n"
              << "Formatos soportados: P2 (PGM escala de grises) y P3 (PPM color RGB).\n";
}

int main(int argc, char* argv[]) {
    // Verificación de flags de ayuda
    if (argc > 1 && (CharUtils::equals(argv[1], "--help") || CharUtils::equals(argv[1], "-h"))) {
        print_usage(argv[0]);
        return 0;
    }

    Image* img = nullptr;
    const char* output_filepath = nullptr;

    if (argc >= 3) {
        // Invocación: ./processor input.ppm output.ppm
        const char* input_filepath = argv[1];
        output_filepath = argv[2];

        std::cerr << "[Processor] Leyendo archivo de entrada: " << input_filepath << "...\n";
        img = ImageIO::read_from_file(input_filepath);

        // Si falló abrir el archivo por argumento, intentar leer de stdin por si se usó redirección
        if (!img) {
            std::cerr << "[Processor] Reintentando lectura desde entrada estandar (stdin)...\n";
            img = ImageIO::read_image(std::cin);
        }
    } else if (argc == 2) {
        // Invocación: ./processor output.ppm < input.ppm
        output_filepath = argv[1];
        std::cerr << "[Processor] Leyendo imagen desde entrada estandar (stdin)...\n";
        img = ImageIO::read_image(std::cin);
    } else {
        // Invocación pura por pipes: ./processor < input.ppm > output.ppm
        std::cerr << "[Processor] Modo stdin -> stdout...\n";
        img = ImageIO::read_image(std::cin);
    }

    if (!img) {
        std::cerr << "[Error] No se pudo cargar la imagen. Verifique el formato (P2 o P3).\n";
        return 1;
    }

    std::cerr << "[Processor] Imagen cargada con exito:\n"
              << "  - Formato:     " << img->get_magic_number() << "\n"
              << "  - Dimensiones: " << img->get_width() << " x " << img->get_height() << "\n"
              << "  - Canales:     " << img->get_channels() << "\n"
              << "  - Max Val:     " << img->get_max_val() << "\n"
              << "  - Total datos: " << img->get_data_size() << " bytes\n";

    // Operación base del Diseño 1: Verificación y clonación
    Image* processed_img = img->clone();

    bool write_success = false;
    if (output_filepath) {
        std::cerr << "[Processor] Escribiendo salida en: " << output_filepath << "...\n";
        write_success = ImageIO::write_to_file(processed_img, output_filepath);
    } else {
        write_success = ImageIO::write_image(processed_img, std::cout);
    }

    if (!write_success) {
        std::cerr << "[Error] Fallo al escribir la imagen de salida.\n";
        delete img;
        delete processed_img;
        return 1;
    }

    std::cerr << "[Processor] Proceso completado exitosamente.\n";

    // Liberación estricta de memoria dinámica
    delete img;
    delete processed_img;

    return 0;
}
