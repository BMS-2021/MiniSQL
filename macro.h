#ifndef MINISQL_MACRO_H
#define MINISQL_MACRO_H

#include <variant>

enum class value_type {
    INT,
    CHAR,  //string
    FLOAT,
};

struct sql_value_type {
    value_type type;
    uint8_t length = 0;

    sql_value_type() = default;
    sql_value_type(value_type type) : type(type) {}
    sql_value_type(uint8_t length) : type(value_type::CHAR), length(length) {}
};

struct schema {
    std::string name;
    sql_value_type type;
    bool unique;

    schema() = default;
    schema(std::string& name, sql_value_type& type, bool unique) : name(name), type(type), unique(unique) {}
};

enum class attribute_operator {
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
};

struct condition {
    std::string attribute_name;
    attribute_operator op;
    std::variant<int, float, std::string> value;

    condition() = default;
    condition(std::string& attribute_name, attribute_operator op, std::variant<int, float, std::string>& value) :
    attribute_name(attribute_name),
    op(op),
    value(value) {}
};

#endif //MINISQL_MACRO_H
