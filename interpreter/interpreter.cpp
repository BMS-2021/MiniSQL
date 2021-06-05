#ifdef READLINE_FOUND
#include <readline/readline.h>
#endif

#include <memory>
#include "../api/api.h"
#include "interpreter.h"
#include "../utils/utils.h"

bool sig_exit = false;
std::unique_ptr<api::base> query_object_ptr = nullptr;

void interpret_entrance() {
    while (!sig_exit) {
        interpreter inter;

        parse(inter.read());

        if (query_object_ptr != nullptr) {
            query_object_ptr->exec();
        }
    }
}

const char* interpreter::read() {
    while (true) {
#ifdef READLINE_FOUND
        char *src = readline(this->start_text());
        if (!first_loop) {
            strcat_s(this->str, "\n", SQL_QUERY_LENGTH);
        }
        strcat_s(this->str, src, SQL_QUERY_LENGTH);
        if (std::strlen(src) >= 1 && src[std::strlen(src) - 1] == ';') {
            std::free(src);
            break;
        }
        std::free(src);
#else
        std::string src;
        std::cout << this->start_text();
        std::getline(std::cin, src);
        if (!first_loop) {
            strcat_s(this->str, "\n", SQL_QUERY_LENGTH);
        }
        strcat_s(this->str, src, SQL_QUERY_LENGTH);
        if (src.length() >= 1 && *src.rbegin() == ';') {
            break;
        }
#endif
        first_loop = false;
    }
    return this->str;
}
