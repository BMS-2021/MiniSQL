#ifndef MINISQL_UTILS_H
#define MINISQL_UTILS_H

#include "../macro.h"

char* strcat_s(char*, const char*, std::size_t);

std::string generate_table_pk_name(const std::string&);

#endif //MINISQL_UTILS_H
