#ifndef MINISQL_API_UTILS_H
#define MINISQL_API_UTILS_H

#include "../macro.h"
#include <vector>

namespace api {
    void throw_on_table_exist(std::string &);
    void throw_on_table_not_exist(std::string &);
    void print_select_result_seperator(const std::vector<unsigned long> &);
    void print_select_result(const std::vector<std::string>&, const result &);
    void validate_condition(const macro::table &, const std::vector<condition> &);
    std::string return_value_name(value_type);
    result convert_sql_tuple_to_result(
        const std::vector<std::string> &, 
        const std::vector<std::string> &,
        const sql_tuple& tup
    );
}

#endif //MINISQL_API_UTILS_H
