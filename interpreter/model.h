#ifndef MINISQL_MODEL_H
#define MINISQL_MODEL_H

#include <string>
#include <vector>
#include <iostream>

#include "../macro.h"

namespace query {
    struct base {
        virtual void exec(){};

        /*
         * If expression evaluates to a pointer to a base class subobject
         * of the object that was allocated with new, the destructor of
         * the base class must be virtual, otherwise the behavior is undefined.
         */
        virtual ~base() = default;
    };

    struct create : base {
        std::string table_name;
    };

    struct create_table final : create {
        std::vector<std::tuple<std::string, value_type>> schema;
    };

    struct create_index final : create {
        std::string index_name;
        std::string element_name;
    };

    struct drop_table final : base {
        std::string table_name;
        inline void exec() override {
            std::cout << "Table to drop: " << table_name << std::endl;
            std::cout << "Interpreter Yes!!!" << std::endl;
        }
    };
}

#endif //MINISQL_MODEL_H
