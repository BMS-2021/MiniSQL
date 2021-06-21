#ifndef MINISQL_API_UTILS_H
#define MINISQL_API_UTILS_H

#include "../macro.h"
#include <vector>

namespace api {
    void throw_on_table_exist(std::string &);
    void throw_on_table_not_exist(std::string &);
    void print_select_result_seperator(const std::vector<unsigned long> &);
    void print_select_result(const macro::table &, const result &);
}

#endif //MINISQL_API_UTILS_H
