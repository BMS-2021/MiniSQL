#ifndef MINISQL_MACRO_H
#define MINISQL_MACRO_H

enum class value_type {
    INT,
    CHAR,  //string
    FLOAT,
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
