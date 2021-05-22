#include <readline/readline.h>
#include <memory>
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
    bool first_loop = true;
    while (true) {
        char *src = nullptr;
        if (first_loop) {
            src = readline("MiniSQL> ");
        } else {
            src = readline("      -> ");
        }
        if (!first_loop) {
            std::strncat(this->str, "\n", SQL_QUERY_LENGTH);
        }
        std::strncat(this->str, src, SQL_QUERY_LENGTH);
        if (std::strlen(src) >= 1 && src[std::strlen(src) - 1] == ';') {
            std::free(src);
            break;
        }
        std::free(src);
        first_loop = false;
    }
    return this->str;
}
