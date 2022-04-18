#ifndef MINISQL_EXCEPTION_H
#define MINISQL_EXCEPTION_H

#include <iostream>
#include <string>

class sql_exception : public std::exception {
    std::string module_name;
    std::string detail;

public:
    unsigned int code;

    sql_exception() = default;
    sql_exception(unsigned int code, std::string& module_name, std::string& detail) :
            code(code), module_name(module_name), detail(detail) {}

    sql_exception(unsigned int code, std::string&& module_name, std::string&& detail) :
            code(code), module_name(module_name), detail(detail) {}

    inline const char* what() const noexcept override {
        return detail.c_str();
    }

    friend std::ostream& operator<<(std::ostream& os, const sql_exception& e);
};

#endif //MINISQL_EXCEPTION_H
