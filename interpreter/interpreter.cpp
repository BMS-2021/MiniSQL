#include <iostream>

#include "model.h"
#include "interpreter.h"

query::base *query_object_ptr = nullptr;

void interpret_entrance() {
    while (true) {
        yyparse();
        auto test = dynamic_cast<query::drop_table *>(query_object_ptr);
        if (test != nullptr) {
            std::cout << "Yes" << std::endl;
        }
    }

}
