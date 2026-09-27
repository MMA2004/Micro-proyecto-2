#ifndef PGM_IMAGE_H
#define PGM_IMAGE_H

#include "Image.h"

/**
 * @brief Especialización de Image para formato PGM (P2, escala de grises, 1 canal).
 */
class PGMImage : public Image {
public:
    /**
     * @brief Constructor con dimensiones y valor máximo.
     */
    PGMImage(int w, int h, int max_v = 255);

    /**
     * @brief Constructor de copia.
     */
    PGMImage(const PGMImage& other);

    /**
     * @brief Destructor.
     */
    ~PGMImage() override = default;

    /**
     * @brief Retorna el identificador mágico "P2".
     */
    const char* get_magic_number() const override;

    /**
     * @brief Crea un clon polimórfico de la imagen PGM.
     */
    Image* clone() const override;

    /**
     * @brief Extrae una región como una nueva imagen PGM.
     */
    Image* extract_region(int start_x, int start_y, int region_w, int region_h) const override;
};

#endif // PGM_IMAGE_H
