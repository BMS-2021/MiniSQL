#ifndef MINISQL_MACRO_H
#define MINISQL_MACRO_H

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

enum class attribute_operator {
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
};

#endif //MINISQL_MACRO_H
