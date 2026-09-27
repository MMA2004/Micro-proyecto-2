#ifndef IMAGE_H
#define IMAGE_H

#include <cstddef>

/**
 * @brief Clase base abstracta para representar imágenes NetPBM (PGM y PPM).
 * Utiliza arreglos dinámicos continuos de memoria (unsigned char*) sin usar std::vector ni std::string.
 */
class Image {
protected:
    int width;
    int height;
    int max_val;
    int channels;          // 1 para PGM (escala de grises), 3 para PPM (color RGB)
    unsigned char* data;   // Arreglo 1D contiguo de tamaño: width * height * channels

public:
    /**
     * @brief Constructor con dimensiones y canales.
     */
    Image(int w, int h, int max_v, int ch);

    /**
     * @brief Destructor virtual que asegura la liberación adecuada del arreglo de datos.
     */
    virtual ~Image();

    /**
     * @brief Constructor de copia profunda.
     */
    Image(const Image& other);

    /**
     * @brief Operador de asignación por copia profunda.
     */
    Image& operator=(const Image& other);

    /**
     * @brief Constructor por movimiento.
     */
    Image(Image&& other) noexcept;

    /**
     * @brief Operador de asignación por movimiento.
     */
    Image& operator=(Image&& other) noexcept;

    // Getters de propiedades básicas
    int get_width() const { return width; }
    int get_height() const { return height; }
    int get_max_val() const { return max_val; }
    int get_channels() const { return channels; }
    int get_total_pixels() const { return width * height; }
    size_t get_data_size() const { return static_cast<size_t>(width) * height * channels; }

    // Acceso directo al buffer continuo (útil para Pthreads, OpenMP y MPI)
    unsigned char* get_raw_data() { return data; }
    const unsigned char* get_raw_data() const { return data; }

    /**
     * @brief Obtiene el valor de un píxel en coordenadas (x, y) y canal c.
     */
    virtual int get_pixel(int x, int y, int c = 0) const;

    /**
     * @brief Modifica el valor de un píxel en coordenadas (x, y) y canal c.
     */
    virtual void set_pixel(int x, int y, int value, int c = 0);

    /**
     * @brief Retorna el número mágico del formato ("P2" o "P3").
     */
    virtual const char* get_magic_number() const = 0;

    /**
     * @brief Crea una copia exacta (clon) polimórfico de la imagen.
     */
    virtual Image* clone() const = 0;

    /**
     * @brief Extrae una sub-región rectangular como una nueva imagen.
     * Esencial para Diseño 3 (división en 4 cuadrantes) y Diseño 4 (MPI).
     */
    virtual Image* extract_region(int start_x, int start_y, int region_w, int region_h) const = 0;

    /**
     * @brief Pega una sub-región en una posición específica de la imagen actual.
     */
    void paste_region(int start_x, int start_y, const Image* region);
};

#endif // IMAGE_H
