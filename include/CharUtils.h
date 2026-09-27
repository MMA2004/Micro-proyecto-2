#ifndef CHAR_UTILS_H
#define CHAR_UTILS_H

#include <cstddef>

/**
 * @brief Utilidades para manejo seguro de cadenas de caracteres (char*)
 * Cumple con la restricción de evitar el uso de std::string.
 */
namespace CharUtils {

    /**
     * @brief Duplica una cadena de caracteres usando memoria dinámica (new char[]).
     * @param src Cadena fuente.
     * @return Puntero a la nueva cadena duplicada, o nullptr si src es nulo.
     */
    char* duplicate(const char* src);

    /**
     * @brief Compara dos cadenas de caracteres.
     * @param a Primera cadena.
     * @param b Segunda cadena.
     * @return true si son idénticas, false en caso contrario.
     */
    bool equals(const char* a, const char* b);

    /**
     * @brief Verifica si una cadena comienza con un prefijo dado.
     * @param str Cadena a verificar.
     * @param prefix Prefijo buscado.
     * @return true si str inicia con prefix.
     */
    bool starts_with(const char* str, const char* prefix);

    /**
     * @brief Verifica si una cadena termina con un sufijo dado.
     * @param str Cadena a verificar.
     * @param suffix Sufijo buscado.
     * @return true si str termina con suffix.
     */
    bool ends_with(const char* str, const char* suffix);

    /**
     * @brief Libera la memoria de una cadena dinámica y coloca el puntero en nullptr.
     * @param str Referencia al puntero de la cadena.
     */
    void free_string(char*& str);

}

#endif // CHAR_UTILS_H
