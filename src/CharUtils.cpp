#include "CharUtils.h"
#include <cstring>

namespace CharUtils {

    char* duplicate(const char* src) {
        if (!src) return nullptr;
        size_t len = std::strlen(src);
        char* copy = new char[len + 1];
        std::strcpy(copy, src);
        return copy;
    }

    bool equals(const char* a, const char* b) {
        if (!a && !b) return true;
        if (!a || !b) return false;
        return std::strcmp(a, b) == 0;
    }

    bool starts_with(const char* str, const char* prefix) {
        if (!str || !prefix) return false;
        size_t len_p = std::strlen(prefix);
        size_t len_s = std::strlen(str);
        if (len_s < len_p) return false;
        return std::strncmp(str, prefix, len_p) == 0;
    }

    bool ends_with(const char* str, const char* suffix) {
        if (!str || !suffix) return false;
        size_t len_s = std::strlen(str);
        size_t len_suffix = std::strlen(suffix);
        if (len_s < len_suffix) return false;
        return std::strcmp(str + len_s - len_suffix, suffix) == 0;
    }

    void free_string(char*& str) {
        if (str) {
            delete[] str;
            str = nullptr;
        }
    }

}
