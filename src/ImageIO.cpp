#include "ImageIO.h"
#include "PGMImage.h"
#include "PPMImage.h"
#include <fstream>
#include <iostream>
#include <cctype>

void ImageIO::skip_comments_and_whitespace(std::istream& in) {
    char ch;
    while (in >> std::ws) {
        if (in.peek() == '#') {
            in.get(ch); // Consume '#'
            while (in.get(ch) && ch != '\n') {
                // Consume el resto de la línea del comentario
            }
        } else {
            break;
        }
    }
}

Image* ImageIO::read_image(std::istream& in) {
    skip_comments_and_whitespace(in);

    char magic[3] = {0, 0, 0};
    if (!(in >> magic[0] >> magic[1])) {
        return nullptr;
    }

    if (magic[0] != 'P' || (magic[1] != '2' && magic[1] != '3')) {
        std::cerr << "Error: Formato no soportado '" << magic[0] << magic[1] 
                  << "'. Se requiere P2 (PGM) o P3 (PPM).\n";
        return nullptr;
    }

    skip_comments_and_whitespace(in);
    int width = 0;
    if (!(in >> width) || width <= 0) {
        std::cerr << "Error: Ancho de imagen invalido.\n";
        return nullptr;
    }

    skip_comments_and_whitespace(in);
    int height = 0;
    if (!(in >> height) || height <= 0) {
        std::cerr << "Error: Alto de imagen invalido.\n";
        return nullptr;
    }

    skip_comments_and_whitespace(in);
    int max_val = 0;
    if (!(in >> max_val) || max_val <= 0) {
        std::cerr << "Error: Maxval invalido.\n";
        return nullptr;
    }

    Image* img = nullptr;
    if (magic[1] == '2') {
        img = new PGMImage(width, height, max_val);
    } else {
        img = new PPMImage(width, height, max_val);
    }

    size_t total_elements = static_cast<size_t>(width) * height * img->get_channels();
    unsigned char* raw_data = img->get_raw_data();

    for (size_t i = 0; i < total_elements; ++i) {
        skip_comments_and_whitespace(in);
        int pixel_val = 0;
        if (!(in >> pixel_val)) {
            std::cerr << "Error: Datos de pixeles incompletos en la posicion " << i << ".\n";
            delete img;
            return nullptr;
        }
        if (pixel_val < 0) pixel_val = 0;
        if (pixel_val > max_val) pixel_val = max_val;
        raw_data[i] = static_cast<unsigned char>(pixel_val);
    }

    return img;
}

Image* ImageIO::read_from_file(const char* filepath) {
    if (!filepath) return nullptr;
    std::ifstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de entrada: " << filepath << "\n";
        return nullptr;
    }
    Image* img = read_image(file);
    file.close();
    return img;
}

bool ImageIO::write_image(const Image* img, std::ostream& out) {
    if (!img || !out.good()) return false;

    out << img->get_magic_number() << "\n";
    out << "# Generado por NetPBM Image Processor - Diseno 1\n";
    out << img->get_width() << " " << img->get_height() << "\n";
    out << img->get_max_val() << "\n";

    const unsigned char* raw_data = img->get_raw_data();
    size_t total_elements = img->get_data_size();
    int items_per_line = (img->get_channels() == 1) ? 16 : 15;

    for (size_t i = 0; i < total_elements; ++i) {
        out << static_cast<int>(raw_data[i]);
        if ((i + 1) % items_per_line == 0 || i + 1 == total_elements) {
            out << "\n";
        } else {
            out << " ";
        }
    }

    return out.good();
}

bool ImageIO::write_to_file(const Image* img, const char* filepath) {
    if (!img || !filepath) return false;
    std::ofstream file(filepath, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de salida: " << filepath << "\n";
        return false;
    }
    bool success = write_image(img, file);
    file.close();
    return success;
}
