#include <iostream>

#include "model.h"
#include "interpreter.h"

bool sig_exit = false;
query::base *query_object_ptr = nullptr;

void interpret_entrance() {
    while (!sig_exit) {
        yyparse();
        auto test = dynamic_cast<query::drop_table*>(query_object_ptr);
        if (test != nullptr) {
            std::cout << "Table to drop: " << test->table_name << std::endl;
            std::cout << "Interpreter Yes!!!" << std::endl;
        }
    }

}
