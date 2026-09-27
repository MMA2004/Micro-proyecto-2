#ifndef PPM_IMAGE_H
#define PPM_IMAGE_H

#include "Image.h"

/**
 * @brief Especialización de Image para formato PPM (P3, RGB color, 3 canales).
 */
class PPMImage : public Image {
public:
    /**
     * @brief Constructor con dimensiones y valor máximo.
     */
    PPMImage(int w, int h, int max_v = 255);

    /**
     * @brief Constructor de copia.
     */
    PPMImage(const PPMImage& other);

    /**
     * @brief Destructor.
     */
    ~PPMImage() override = default;

    /**
     * @brief Retorna el identificador mágico "P3".
     */
    const char* get_magic_number() const override;

    /**
     * @brief Crea un clon polimórfico de la imagen PPM.
     */
    Image* clone() const override;

    /**
     * @brief Extrae una región como una nueva imagen PPM.
     */
    Image* extract_region(int start_x, int start_y, int region_w, int region_h) const override;
};

#endif // PPM_IMAGE_H
