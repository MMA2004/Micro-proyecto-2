#ifndef IMAGE_IO_H
#define IMAGE_IO_H

#include "Image.h"
#include <istream>
#include <ostream>

/**
 * @brief Clase utilitaria para operaciones de Entrada/Salida de imágenes NetPBM (PPM y PGM).
 * Soporta streams estándar (std::cin, std::cout) y archivos en disco.
 * Cumple con el manejo de comentarios '#' y la regla de no usar std::string.
 */
class ImageIO {
public:
    /**
     * @brief Lee una imagen (P2 o P3) desde un stream de entrada.
     * @param in Stream de entrada (por ejemplo, std::cin o std::ifstream).
     * @return Puntero polimórfico a Image (PGMImage o PPMImage), o nullptr en caso de error.
     */
    static Image* read_image(std::istream& in);

    /**
     * @brief Lee una imagen desde un archivo en disco.
     * @param filepath Ruta del archivo como puntero a char.
     * @return Puntero polimórfico a Image, o nullptr en caso de error.
     */
    static Image* read_from_file(const char* filepath);

    /**
     * @brief Escribe una imagen en un stream de salida.
     * @param img Puntero a la imagen a escribir.
     * @param out Stream de salida (por ejemplo, std::cout o std::ofstream).
     * @return true si la escritura fue exitosa, false en caso contrario.
     */
    static bool write_image(const Image* img, std::ostream& out);

    /**
     * @brief Escribe una imagen en un archivo en disco.
     * @param img Puntero a la imagen a escribir.
     * @param filepath Ruta de destino como puntero a char.
     * @return true si la escritura fue exitosa, false en caso contrario.
     */
    static bool write_to_file(const Image* img, const char* filepath);

    /**
     * @brief Salta espacios en blanco y cualquier comentario '#' hasta el siguiente dato válido.
     * @param in Stream de entrada.
     */
    static void skip_comments_and_whitespace(std::istream& in);
};

#endif // IMAGE_IO_H
