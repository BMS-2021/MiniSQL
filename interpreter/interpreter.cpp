#include <iostream>

#include "model.h"
#include "interpreter.h"

bool sig_exit = false;
query::base *query_object_ptr = nullptr;

void interpret_entrance() {
    while (!sig_exit) {
        yyparse();
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
