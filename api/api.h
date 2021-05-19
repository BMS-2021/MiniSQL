#ifndef MINISQL_API_H
#define MINISQL_API_H

#include <string>
#include <vector>

#include "../macro.h"

namespace api {
    void select_table(
            std::string &table,
            std::vector<std::string> attribute_list,
            std::vector<std::tuple<
                    std::string&,  // attribute
                    attribute_operator,
                    std::pair<std::string&, value_type>  // value
            >> condition_list
    );

    void update_table(
            std::string &table,
            std::string &attribute,
            std::pair<std::string&, value_type> value,
            std::vector<std::tuple<
                    std::string&,  // attribute
                    attribute_operator,
                    std::pair<std::string&, value_type>  // value
            >> condition_list
    );

    void insert_table(
            std::string &table,
            std::vector<std::pair<std::string&, value_type>> value_list
    );

    void create_table(
            std::string &table,
            std::vector<std::pair<std::string&, value_type>> schema_list,
            std::string &primary_key = (std::string &)""
    );

    void create_index(
            std::string &table,
            std::string &attribute,
            std::string &index
    );

    void drop_table(std::string &table);

    void drop_index(std::string &index);
}

#endif //MINISQL_API_H
