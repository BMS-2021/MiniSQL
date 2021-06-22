#include "utils.h"
#include "exception.h"

char* strcat_s(char* dest, const char* src, std::size_t max_length) {
    if (std::strlen(dest) + std::strlen(src) > max_length) {
        throw sql_exception(900, "utilities", "input buffer overflow");
    }

    return std::strcat(dest, src);
}


