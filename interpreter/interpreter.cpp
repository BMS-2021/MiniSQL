#include <iostream>

#include "model.h"
#include "interpreter.h"

bool sig_exit = false;
query::base *query_object_ptr = nullptr;

void interpret_entrance() {
    while (!sig_exit) {
        yyparse();
        query_object_ptr->exec();
    }
}
