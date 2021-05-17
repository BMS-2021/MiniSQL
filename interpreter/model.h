#ifndef MINISQL_MODEL_H
#define MINISQL_MODEL_H

#include <string>
#include <vector>

#include "../macro.h"

namespace query {
    struct base {
        virtual void placeholder(){};
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
    };
}

#endif //MINISQL_MODEL_H
