#ifdef READLINE_FOUND
#include <readline/readline.h>
#endif

#include "model.h"
#include "interpreter.h"

bool sig_exit = false;
query::base *query_object_ptr = nullptr;

void interpret_entrance() {
    while (!sig_exit) {
        interpreter inter;

        parse(inter.read());

        if (query_object_ptr != nullptr) {
            query_object_ptr->exec();
        }

        /*
         * If expression evaluates to a null pointer value,
         * no destructors are called, and the deallocation
         * function may or may not be called (it's unspecified),
         * but the default deallocation functions are guaranteed
         * to do nothing when passed a null pointer.
         */
        delete query_object_ptr;
        query_object_ptr = nullptr;
    }
}

const char* interpreter::read() {
    while (true) {
#ifdef READLINE_FOUND
        char *src = readline(this->start_text());
        if (!first_loop) {
            std::strncat(this->str, "\n", SQL_QUERY_LENGTH);
        }
        std::strncat(this->str, src, SQL_QUERY_LENGTH);
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
            std::strncat(this->str, "\n", SQL_QUERY_LENGTH);
        }
        std::strncat(this->str, src.c_str(), SQL_QUERY_LENGTH);
        if (src.length() >= 1 && *src.rbegin() == ';') {
            break;
        }
#endif
        first_loop = false;
    }
    return this->str;
}
